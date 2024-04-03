#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <signal.h>

#include <sys/inotify.h>
#include <libnotify/notify.h>

#define EXT_SUCCESS 0
#define EXT_ERR_TOO_FEW_ARGS 1
#define EXT_INIT_INOTIFY 2
#define EXT_ERR_ADD_WATCH 3
#define EXT_ERR_BASEPATH_NULL 4
#define EXT_ERR_READ_INOTIFY 5

int IeventQueue = -1;
int IeventStatus = -1;
int main(int argc, char** argv){

  char *basePath = NULL;
  char *token = NULL;
  char *notificationMessage = NULL;

  char buffer[4096];
  int readLength;

  const struct inotify_event* watchEvent;
  
  notify_init ("I'm watching you!");
  NotifyNotification * Message = notify_notification_new ("Filehawk", "I'm watching you!", "dialog-information");
  
  const uint32_t watchMask = IN_CREATE | IN_DELETE | IN_ACCESS | IN_CLOSE_WRITE | IN_MODIFY | IN_MOVE_SELF;

  if (argc < 2){
    fprintf(stderr, "USAGE: filehawk PATH");
    exit(EXT_ERR_TOO_FEW_ARGS);
  }

  basePath = (char *)malloc(sizeof(char)*(strlen(argv[1]) + 1)); //allocate space that is size of char data type * length of input path
  strcpy(basePath, argv[1]);

  token = strtok(basePath, "/");
  while (token != NULL){ //navigate through till you find the NULL pointer, which is the ending of the string
    basePath = token;
    token = strtok(NULL, "/");
  }

  if (basePath == NULL){
    fprintf(stderr, "Error getting basepath.\n");
  }
  IeventQueue = inotify_init();
  if(IeventQueue  == -1){
    fprintf(stderr, "Error intialising inotify instance.\n");
    exit(EXT_INIT_INOTIFY);
  }

  IeventStatus = inotify_add_watch(IeventQueue, argv[1], watchMask);
  if(IeventStatus == -1){
    fprintf(stderr,"Error adding file to watch instance.\n");
    exit(EXT_ERR_ADD_WATCH);
  }

  while(true){
    printf("Waiting for ievent....\n");

    readLength = read(IeventQueue, buffer, sizeof(buffer));
    if(readLength == -1){
      fprintf(stderr, "Error reading from inotify instance.\n");
      exit(EXT_ERR_READ_INOTIFY);
    }

    for(char *bufferPointer = buffer; bufferPointer < buffer + readLength; bufferPointer += sizeof(struct inotify_event) + watchEvent->len){

      notificationMessage = NULL;
      watchEvent = (const struct inotify_event *) bufferPointer;

      if (watchEvent->mask & IN_CREATE){
        notificationMessage = "File Created.\n";
      }

      if (watchEvent->mask & IN_DELETE){
        notificationMessage = "File Deleted.\n";
      }

      if (watchEvent->mask & IN_ACCESS){
        notificationMessage = "File Accessed.\n";
      }

      if (watchEvent->mask & IN_CLOSE_WRITE){
        notificationMessage = "File written and close.\n";
      }

      if (watchEvent->mask & IN_MODIFY){
        notificationMessage = "File modified.\n";
      }

      if (watchEvent->mask & IN_MOVE_SELF){
        notificationMessage = "File moved.\n";
      }

      if (notificationMessage == NULL){
        continue;
      }
      Message = notify_notification_new ("Alert!", notificationMessage , "dialog-information");
      notify_notification_show (Message, NULL);
      g_object_unref(G_OBJECT(Message));

      printf("%s\n", notificationMessage);
    }
  }
  exit(EXT_SUCCESS);
}
