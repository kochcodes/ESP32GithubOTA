#include <ESP32GithubOTA.h>
#include <Update.h>

ESP32GithubOTA::ESP32GithubOTA() {}
ESP32GithubOTA::ESP32GithubOTA(WiFiClientSecure &c, int led_pin)
{
    this->client = c;
    this->led_pin = led_pin;
}
void ESP32GithubOTA::onStart(githubOTAStartCallback cb)
{
    _StartCB = cb;
}
void ESP32GithubOTA::onEnd(githubOTAEndCallback cb)
{
    _EndCB = cb;
}
void ESP32GithubOTA::onError(githubOTAErrorCallback cb)
{
    _ErrorCB = cb;
}
void ESP32GithubOTA::onProgress(githubOTAProgressCallback cb)
{
    _ProgressCB = cb;
}

bool ESP32GithubOTA::check()
{
    String md5 = updateAvailable();
    if (md5.length() != 32)
        Serial.println("MD5 not found!");
    else if (md5 != ESP.getSketchMD5())
    {
        String sha = getBlobSHA();
        runFirmwareUpdate(sha);
    }
    else
        Serial.println("Firmeware up to date");
}

String ESP32GithubOTA::getBlobSHA()
{
    if (!client.connect("api.github.com", 443))
        Serial.println("Connection failed!");
    else
    {
        client.println("GET /repos/" + String(owner) + "/" + String(repository) + "/contents?ref=" + String(branch) + " HTTP/1.0");
        client.println("Host: api.github.com");
        client.println("User-Agent: ESP32");
        client.println("Accept: application/vnd.github.v3+json");
        client.println("Authorization: token " + String(token));
        client.println("Connection: Close");
        client.println();

        while (client.connected())
        {
            String line = client.readStringUntil('\n');
            if (line == "\r")
            {
                break;
            }
        }
        String sha = "";
        while (client.available())
        {
            String line = client.readStringUntil('\n');
            int index = line.indexOf("\"path\":\"firmware.bin\"");
            if (index >= 0)
            {
                sha = line.substring(index + 29, index + 29 + 40);
            }
        }
        client.stop();
        return sha;
    }
    return "";
}
String ESP32GithubOTA::updateAvailable(void)
{
    if (!client.connect("api.github.com", 443))
        Serial.println("Connection failed!");
    else
    {
        client.println("GET /repos/kochcodes/ESP32GithubOTA/contents/info.txt?ref=release HTTP/1.0");
        client.println("Host: api.github.com");
        client.println("User-Agent: ESP32");
        client.println("Accept: application/vnd.github.v3.raw");
        client.println("Authorization: token 8f46bf08a4cdee1bf11bcf019b520cdcea7c2cea");
        client.println("Connection: close");
        client.println();

        while (client.connected())
        {
            String line = client.readStringUntil('\n');
            if (line == "\r")
            {
                break;
            }
        }
        // if there are incoming bytes available
        // from the server, read them and print them:
        int counter = 32;
        String md5 = "";
        while (client.available() && counter-- > 0)
        {
            md5 += (char)client.read();
        }
        client.stop();
        if (md5.length() == 32)
            return md5;
        else
            return "";
    }
    return "";
}
bool ESP32GithubOTA::runUpdate(Stream &in, uint32_t size)
{
    if (!Update.begin(size, U_FLASH))
    {
        return false;
    }

    if (Update.writeStream(in) != size)
    {
        return false;
    }

    if (!Update.end())
    {
        return false;
    }
    return true;
}
HTTPGithubResult ESP32GithubOTA::runFirmwareUpdate(String &blob)
{
    HTTPGithubResult ret = HTTP_GITHUB_UPDATE_FAILED;
    if (!client.connect("api.github.com", 443))
        Serial.println("Connection failed!");
    else
    {
        client.println("GET /repos/kochcodes/ESP32GithubOTA/git/blobs/" + blob + "?ref=release HTTP/1.0");
        client.println("Host: api.github.com");
        client.println("User-Agent: ESP32");
        client.println("Accept: application/vnd.github.v3.raw");
        client.println("Authorization: token 8f46bf08a4cdee1bf11bcf019b520cdcea7c2cea");
        client.println("Connection: Keep-Alive");
        client.println("Keep-Alive: timeout=60, max=1000");
        client.println();

        int len = 0;
        while (client.connected())
        {
            String line = client.readStringUntil('\n');
            if (line.indexOf("Content-Length:") >= 0)
            {
                len = atoi(line.substring(15).c_str());
            }
            if (line == "\r")
            {
                break;
            }
        }
        Serial.printf("Firmwaresize:    %d\n", len);
        Serial.printf("Available space: %d\n", ESP.getFreeSketchSpace());
        if (runUpdate(client, len))
        {
            client.stop();
            ESP.restart();
            ret = HTTP_GITHUB_UPDATE_OK; // should never be reached
        }
        else
        {
            Serial.println("Update failed!");
            ret = HTTP_GITHUB_UPDATE_FAILED;
        }

        client.stop();
    }
    return ret;
}

void setAccessToken(const char *token)
{
    token = token;
}
void setRepository(const char *owner, const char *repos, const char *branch)
{
    owner = owner;
    repos = repos;
    branch = branch;
}