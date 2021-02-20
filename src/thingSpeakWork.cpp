/* Copyright (C) 2019-2021 Lee C. Bussy (@LBussy)

This file is part of Lee Bussy's Brew Bubbles (brew-bubbles).

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE. */

#include "thingSpeakWork.h"

/*
	Function: createChannel

	Summary:
	Create a Thingspeak channel

	Parameters:
	const char *key - Thingspeak account API key
    const char *bubName - Name of the current Brew Bubble

	Returns:
	Channel number if successful, null string on failure

*/

// #define OK_SUCCESS 200				 // OK / Success
#define ERR_BADAPIKEY 400			 // Incorrect API key (or invalid ThingSpeak server address)
// #define ERR_BADURL 404				 // Incorrect API key (or invalid ThingSpeak server address)
// #define ERR_OUT_OF_RANGE -101		 // Value is out of range or string is too long (> 255 bytes)
// #define ERR_INVALID_FIELD_NUM -201	 // Invalid field number specified
#define ERR_SETFIELD_NOT_CALLED -210 // setField() was not called before writeFields()
// #define ERR_CONNECT_FAILED -301		 // Failed to connect to ThingSpeak
// #define ERR_UNEXPECTED_FAIL -302	 // Unexpected failure during write to ThingSpeak
// #define ERR_BAD_RESPONSE -303		 // Unable to parse response
// #define ERR_TIMEOUT -304			 // Timeout waiting for server to respond
// #define ERR_NOT_INSERTED -401		 // Point was not inserted (most probable cause is the rate limit of once every 15 seconds)

void getNewBBChannel(String &writeKey, const char *key, const char *bubName)
{
    // TODO:  Check for: Return (402): {"status":"402","error":"Payment required."}
    String postData;

    // Name the Channel
    char name[65];
    sprintf(name, "%s | %s", API_KEY, bubName);

    // Create channel description
    char description[129];
    sprintf(description, "%s data for %s.", API_KEY, bubName);

    // Name channel fields
    char tempFormat[1];
    if (config.bubble.tempinf)
    {
        tempFormat[0] = 'F';
    }
    else
    {
        tempFormat[0] = 'F';
    }

    char fieldAmb[11];
    sprintf(fieldAmb, "Ambient °%s", tempFormat);

    char fieldVes[10];
    sprintf(fieldVes, "Vessel °%s", tempFormat);

    const char *field[] = {"BPM", fieldAmb, fieldVes};

    String retString = "";
    int retVal = createChannel(
        retString,
        key,
        name,
        sizeof(field) / sizeof(field[0]),
        field,
        description,
        true,
        NULL,
        "https://www.brewbubbles.com",
        NULL);

    if (retVal == 200)
    {
        const size_t capacity = JSON_ARRAY_SIZE(0) + JSON_ARRAY_SIZE(2) + 2 * JSON_OBJECT_SIZE(2) + JSON_OBJECT_SIZE(16) + 360;
        DynamicJsonDocument doc(capacity);

        DeserializationError err = deserializeJson(doc, retString);

        if (err)
        {
            Log.error(F("Error: deserializeJson() returned: %s."), err.c_str());
            return;
        }

        for (int i = 0; i < 2; i++)
        {
            if (doc["api_keys"][i]["write_flag"] == true)
            {
                const char *wf = doc["api_keys"][i]["api_key"];
                writeKey = wf;
            }
        }
        return;
    }
    else
    {
        Log.error(F("ThingSpeak POST returned value %d." CR), retVal);
    }
}

/*
	Function: createChannel

	Summary:
	Create a Thingspeak channel

	Parameters:
	String &channelJSON - (Required, by Reference) Returns the write key JSON
	const char * api_key - (Required) Specify User API Key, which you can find in your profile. This key is different from the channel API keys.
	const char * field - Array of field names.
	const char * name - (Optional) Name of the channel.
	const char * description - (Optional) Specify an ID or name for the client making the request.
	const bool * public_flag - (Optional) Whether the channel is public. The default is false.
	const char * tags - (Optional) Comma-separated list of tags.
	const char * url - (Optional) Web page URL for the channel.
	const char * metadata - (Optional) Metadata for the channel, which can include JSON, XML, or any other data.

	Returns:
	Channel number if successful.
	0 if attempt failed

	*/

int createChannel(
    String &channelJSON,
    const char *api_key,
    const char *name,
    const unsigned long fieldCount,
    const char *field[],
    const char *description,
    const bool public_flag,
    const char *tags,
    const char *url,
    const char *metadata)
{
    WiFiClient client;
    HTTPClient http;

    // Create POST data
    String postData;

    if (!strlen(api_key))
        return ERR_BADAPIKEY;
    postData += "api_key=" + String(api_key);

    if (strlen(name))
        postData += "&name=" + urlencode(String(name));

    // Handle field names
    String fieldData = "";
    for (unsigned int i = 0; i < fieldCount; i++)
    {
        fieldData += "&";
        char buffer[4];
        sprintf(buffer, "%d", i + 1);
        String inst = buffer;
        fieldData += "field" + inst + "=" + urlencode(String(field[i]));
    }
    if (fieldData.length() == 0)
        return ERR_SETFIELD_NOT_CALLED;
    postData += fieldData;

    if (strlen(description))
        postData += "&description=" + urlencode(String(description));

    if (public_flag)
        postData += "&public_flag=true";

    if (tags && strlen(tags))
        postData += "&tags=" + urlencode(String(tags));

    if (url && strlen(url))
        postData += "&url=" + urlencode(String(url));

    if (metadata && strlen(metadata))
        postData += "&metadata=" + urlencode(String(metadata));

    String tsURL = "http://" + String(THINGSPEAK_URL) + ":" + THINGSPEAK_PORT_NUMBER + "/channels.json";
    http.begin(client, tsURL); //Specify request destination
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");

    int httpCode = http.POST(postData);
    channelJSON = http.getString();

#ifdef PRINT_DEBUG_MESSAGES
    Serial.println(F("Create ThingSpeak Channel results:"));
    Serial.println(httpCode);    //Print HTTP return code
    Serial.println(channelJSON); //Print request response payload
#endif

    http.end(); //Close connection
    return httpCode;
};

String urldecode(String stringToProcess)
{

    String encodedString = "";
    char chr;
    char code0;
    char code1;
    for (unsigned int i = 0; i < stringToProcess.length(); i++)
    {
        chr = stringToProcess.charAt(i);
        if (chr == '+')
        {
            encodedString += ' ';
        }
        else if (chr == '%')
        {
            i++;
            code0 = stringToProcess.charAt(i);
            i++;
            code1 = stringToProcess.charAt(i);
            chr = (h2int(code0) << 4) | h2int(code1);
            encodedString += chr;
        }
        else
        {

            encodedString += chr;
        }

        yield();
    }

    return encodedString;
}

String urlencode(String stringToProcess)
{
    String encodedString = "";
    char chr;
    char code0;
    char code1;
    for (unsigned int i = 0; i < stringToProcess.length(); i++)
    {
        chr = stringToProcess.charAt(i);
        if (chr == ' ')
        {
            encodedString += '+';
        }
        else if (isalnum(chr))
        {
            encodedString += chr;
        }
        else
        {
            code1 = (chr & 0xf) + '0';
            if ((chr & 0xf) > 9)
            {
                code1 = (chr & 0xf) - 10 + 'A';
            }
            chr = (chr >> 4) & 0xf;
            code0 = chr + '0';
            if (chr > 9)
            {
                code0 = chr - 10 + 'A';
            }
            encodedString += '%';
            encodedString += code0;
            encodedString += code1;
        }
        yield();
    }
    return encodedString;
}

unsigned char h2int(char chr)
{
    if (chr >= '0' && chr <= '9')
    {
        return ((unsigned char)chr - '0');
    }
    if (chr >= 'a' && chr <= 'f')
    {
        return ((unsigned char)chr - 'a' + 10);
    }
    if (chr >= 'A' && chr <= 'F')
    {
        return ((unsigned char)chr - 'A' + 10);
    }
    return (0);
}
