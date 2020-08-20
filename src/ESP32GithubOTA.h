#ifndef __ESP32_GITHUB_OTA__
#define __ESP32_GITHUB_OTA__

#include <Arduino.h>
#include <WiFiClientSecure.h>

enum HTTPGithubResult
{
    HTTP_GITHUB_UPDATE_FAILED,
    HTTP_GITHUB_UPDATE_NO_UPDATES,
    HTTP_GITHUB_UPDATE_OK
};

using githubOTAStartCallback = std::function<void()>;
using githubOTAEndCallback = std::function<void()>;
using githubOTAErrorCallback = std::function<void(int)>;
using githubOTAProgressCallback = std::function<void(int, int)>;

class ESP32GithubOTA
{
public:
    ESP32GithubOTA();
    ESP32GithubOTA(WiFiClientSecure &, int);
    bool check();

    void setAccessToken(char *token);
    void setRepository(char *owner, char *repos, char *branch);

    void onStart(githubOTAStartCallback);
    void onEnd(githubOTAEndCallback);
    void onError(githubOTAErrorCallback);
    void onProgress(githubOTAProgressCallback);

private:
    WiFiClientSecure client;
    int led_pin;
    const char *host = "api.github.com";
    char *token;
    char *owner;
    char *repository;
    char *branch;

    githubOTAStartCallback _StartCB;
    githubOTAEndCallback _EndCB;
    githubOTAErrorCallback _ErrorCB;
    githubOTAProgressCallback _ProgressCB;

    String updateAvailable(void);
    String getBlobSHA();
    HTTPGithubResult runFirmwareUpdate(String &);
    bool runUpdate(Stream &, uint32_t);
};

#endif