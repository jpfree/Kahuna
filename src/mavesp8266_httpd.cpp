/****************************************************************************
 *
 * Copyright (c) 2015, 2016 Gus Grubba. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 ****************************************************************************/

/**
 * @file mavesp8266_httpd.cpp
 * ESP8266 Wifi AP, MavLink UART/UDP Bridge
 *
 * @author Gus Grubba <mavlink@grubba.com>
 */

#include <ESP8266WebServer.h>

#include "mavesp8266.h"
#include "mavesp8266_httpd.h"
#include "mavesp8266_parameters.h"
#include "mavesp8266_gcs.h"
#include "mavesp8266_vehicle.h"

const char PROGMEM kTEXTPLAIN[] = "text/plain";
const char PROGMEM kTEXTHTML[] = "text/html";
const char PROGMEM kACCESSCTL[] = "Access-Control-Allow-Origin";
const char PROGMEM kUPLOADFORM[] = "<h1><a href='/'>MAVLink WiFi Bridge</a></h1><form method='POST' action='/upload' enctype='multipart/form-data'><input type='file' name='update'><br><input type='submit' value='Update'></form>";
// const char PROGMEM kHEADER[] = "<!DOCTYPE html><html><head><style> #title {color: red;} </style><title>MavLink Bridge</title></head><body><h1 id=\"title\"><a href='/'>MAVLink WiFi Bridge</a></h1>";
const char PROGMEM kHEADER[] = "<!doctypehtml><meta content='text/html; charset=windows-1252'http-equiv=Content-Type><style>body{margin:0;font-family:Arial,Helvetica,sans-serif}.topnav{overflow:hidden;background-color:#333}.topnav a{float:left;display:block;color:#f2f2f2;text-align:center;padding:14px 16px;text-decoration:none;font-size:20px}.topnav a:hover{background-color:#ddd;color:#000}.topnav a.active{background-color:#61206e;color:#fff}.topnav .icon{display:none}@media screen and (max-width:600px){.topnav a:not(:first-child){display:none}.topnav a.icon{float:right;display:block}}@media screen and (max-width:600px){.topnav.responsive{position:relative}.topnav.responsive .icon{position:absolute;right:0;top:0}.topnav.responsive a{float:none;display:block;text-align:left}}.container{display:inline-block;cursor:pointer}.bar1,.bar2,.bar3{width:20px;height:3px;background-color:#fff;margin:2px 0;transition:.4s}.change .bar1{transform:translate(0,5px) rotate(-45deg)}.change .bar2{opacity:0}.change .bar3{transform:translate(0,-5px) rotate(45deg)}.formbox{max-width:50%;margin:20px;padding:20px;border:1px solid #ccc;border-radius:5px;background-color:#f2f2f2}@media screen and (max-width:600px){.formbox{margin:20px 2px;max-width:100%}}#logo{height:10mm;float:left;margin-bottom:0}#title{display:block;float:left;margin:0;padding:0;font-size:30px;color:#61206e}#logocontainer{display:flex;width:100%;height:fit-content;margin:0;padding:0;background-color:#f2f2f2;padding:5px}</style><title>BR Kahuna</title><div id=logocontainer><svg id=logo id=svg32909 inkscape:export-filename=LogoColourNoBackgroundV2.png inkscape:export-xdpi=600 inkscape:export-ydpi=600 inkscape:version='1.2 (dc2aedaf03, 2022-05-15)'sodipodi:docname=LogoColourV2.svg version=1.1 viewBox='0 0 250 100'xmlns=http://www.w3.org/2000/svg xmlns:inkscape=http://www.inkscape.org/namespaces/inkscape xmlns:sodipodi=http://sodipodi.sourceforge.net/DTD/sodipodi-0.dtd xmlns:svg=http://www.w3.org/2000/svg><sodipodi:namedview bordercolor=#666666 borderopacity=1.0 id=namedview32911 inkscape:current-layer=layer1 inkscape:cx=483.47589 inkscape:cy=512.90486 inkscape:deskcolor=#d1d1d1 inkscape:document-units=mm inkscape:pagecheckerboard=0 inkscape:pageopacity=0.0 inkscape:showpageshadow=2 inkscape:window-height=987 inkscape:window-maximized=1 inkscape:window-width=1680 inkscape:window-x=1912 inkscape:window-y=472 inkscape:zoom=0.5946522 pagecolor=#ffffff showgrid=false /><defs id=defs32906 /><g id=layer1 transform=translate(3.6199529,20.395338) inkscape:groupmode=layer inkscape:label='Layer 1'><g id=g3205 transform=translate(-0.62316481,-4.3941031)><g id=g1240 transform=matrix(1.8479962,0,0,1.8479962,-9.707708,-3.7351352)><path d='M 12.448179,35.431994 30.470046,17.816324'id=path19882 style=fill:none;fill-opacity:1;stroke:#61206e;stroke-width:2;stroke-linecap:butt;stroke-linejoin:miter;stroke-miterlimit:4;stroke-dasharray:none;stroke-opacity:1 /><g id=g29741 transform=matrix(0.26458333,0,0,0.26458333,3.6431699,-4.4744644) style=fill:none;fill-opacity:1;stroke:#61206e;stroke-width:7.55906;stroke-dasharray:none;stroke-opacity:1><path d='M 33.278774,150.82756 V 37.441725 h 37.795282 c 0,0 20.650751,-1.400322 32.600624,15.97814 11.94987,17.37846 -2.28176,30.828785 -2.28176,30.828785 l -68.114146,66.57891'id=path21239 style=fill:none;fill-opacity:1;stroke:#61206e;stroke-width:7.55906;stroke-linecap:butt;stroke-linejoin:miter;stroke-miterlimit:4;stroke-dasharray:none;stroke-opacity:1 sodipodi:nodetypes=cccscc /><g id=g24573 style=fill:none;fill-opacity:1;stroke:#61206e;stroke-width:7.55906;stroke-dasharray:none;stroke-opacity:1><path d='m 71.074056,37.441725 43.626044,-0.08317 c 0,0 20.65075,-1.400321 32.60062,15.97814 11.94987,17.378461 -2.28176,30.828785 -2.28176,30.828785 l -28.34746,27.14174'id=path21239-9 style=fill:none;fill-opacity:1;stroke:#61206e;stroke-width:7.55906;stroke-linecap:butt;stroke-linejoin:miter;stroke-miterlimit:4;stroke-dasharray:none;stroke-opacity:1 sodipodi:nodetypes=cczcc /><path d='M 33.278774,150.82756 155.5635,150.74439 125.34163,103.47303'id=path23738 style=fill:none;fill-opacity:1;stroke:#61206e;stroke-width:7.55906;stroke-linecap:butt;stroke-linejoin:miter;stroke-miterlimit:4;stroke-dasharray:none;stroke-opacity:1 sodipodi:nodetypes=ccc /></g></g><path d='m 11.448179,35.428067 10.870461,0.0039 c 0,0 10.549494,0.227312 12.21375,-6.589313 1.664256,-6.816624 -5.727061,-9.384571 -5.727061,-9.384571'id=path21675 style=fill:none;fill-opacity:1;stroke:#61206e;stroke-width:2;stroke-linecap:butt;stroke-linejoin:miter;stroke-miterlimit:3.1;stroke-dasharray:none;stroke-opacity:1 sodipodi:nodetypes=cczc /></g><text id=text430 style='font-style:normal;font-variant:normal;font-weight:400;font-stretch:normal;font-size:136.063px;line-height:1.25;font-family:&#39;Times New Roman&#39;;-inkscape-font-specification:&#39;Times New Roman, &#39;;font-variant-ligatures:normal;font-variant-position:normal;font-variant-caps:normal;font-variant-numeric:normal;font-variant-alternates:normal;font-variant-east-asian:normal;font-feature-settings:normal;font-variation-settings:normal;text-indent:0;text-align:start;text-decoration-line:none;text-decoration-style:solid;text-decoration-color:#000;letter-spacing:normal;word-spacing:normal;text-transform:none;writing-mode:lr-tb;direction:ltr;text-orientation:mixed;dominant-baseline:auto;baseline-shift:baseline;text-anchor:start;white-space:normal;shape-padding:0;shape-margin:0;inline-size:0;opacity:1;fill:#61206e;fill-opacity:1;stroke:none;stroke-width:.999999;stroke-linecap:butt;stroke-linejoin:miter;stroke-miterlimit:4;stroke-dasharray:none;stroke-dashoffset:0;stroke-opacity:1;stop-color:#000;stop-opacity:1'transform=matrix(0.26458333,0,0,0.26458333,48.792936,14.502967) x=125.12414 xml:space=preserve y=57.077274><tspan id=tspan428 sodipodi:role=line style='font-style:normal;font-variant:normal;font-weight:400;font-stretch:normal;font-size:136.063px;font-family:&#39;Space Mono&#39;;-inkscape-font-specification:&#39;Space Mono&#39;;fill:#61206e;fill-opacity:1'x=125.12414 y=57.077274>Beyond</tspan></text><text id=text955 style='font-style:normal;font-variant:normal;font-weight:400;font-stretch:normal;font-size:136.063px;line-height:1.25;font-family:&#39;Times New Roman&#39;;-inkscape-font-specification:&#39;Times New Roman, &#39;;font-variant-ligatures:normal;font-variant-position:normal;font-variant-caps:normal;font-variant-numeric:normal;font-variant-alternates:normal;font-variant-east-asian:normal;font-feature-settings:normal;font-variation-settings:normal;text-indent:0;text-align:start;text-decoration-line:none;text-decoration-style:solid;text-decoration-color:#000;letter-spacing:normal;word-spacing:normal;text-transform:none;writing-mode:lr-tb;direction:ltr;text-orientation:mixed;dominant-baseline:auto;baseline-shift:baseline;text-anchor:start;white-space:normal;shape-padding:0;shape-margin:0;inline-size:0;opacity:1;fill:#000;fill-opacity:1;stroke:none;stroke-width:.999999;stroke-linecap:butt;stroke-linejoin:miter;stroke-miterlimit:4;stroke-dasharray:none;stroke-dashoffset:0;stroke-opacity:1;stop-color:#000;stop-opacity:1'transform=matrix(0.26458333,0,0,0.26458333,-120.6662,50.185669) x=761.78986 xml:space=preserve y=48.768002><tspan id=tspan953 sodipodi:role=line style='font-style:normal;font-variant:normal;font-weight:400;font-stretch:normal;font-size:136.063px;font-family:&#39;Space Mono&#39;;-inkscape-font-specification:&#39;Space Mono&#39;'x=761.78986 y=48.768002>Robotix</tspan></text></g></g></svg><h1 id=title>Kahuna</h1></div><script>function changeTopnav(){var a=document.getElementById('BRtopnav');'topnav'===a.className?a.className+=' responsive':a.className='topnav'}</script>";
const char PROGMEM kBADARG[] = "BAD ARGS";
const char PROGMEM kAPPJSON[] = "application/json";

const char *kBAUD = "baud";
const char *kPWD = "pwd";
const char *kSSID = "ssid";
const char *kPWDSTA = "pwdsta";
const char *kSSIDSTA = "ssidsta";
const char *kIPSTA = "ipsta";
const char *kGATESTA = "gatewaysta";
const char *kSUBSTA = "subnetsta";
const char *kCPORT = "cport";
const char *kHPORT = "hport";
const char *kCHANNEL = "channel";
const char *kDEBUG = "debug";
const char *kREBOOT = "reboot";
const char *kPOSITION = "position";
const char *kMODE = "mode";

const char *kFlashMaps[7] = {
    "512KB (256/256)",
    "256KB",
    "1MB (512/512)",
    "2MB (512/512)",
    "4MB (512/512)",
    "2MB (1024/1024)",
    "4MB (1024/1024)"};

static uint32_t flash = 0;
static char paramCRC[12] = {""};

ESP8266WebServer webServer(80);
MavESP8266Update *updateCB = NULL;
bool started = false;

//---------------------------------------------------------------------------------
void setNoCacheHeaders()
{
    webServer.sendHeader("Cache-Control", "no-cache, no-store, must-revalidate");
    webServer.sendHeader("Pragma", "no-cache");
    webServer.sendHeader("Expires", "0");
}

//---------------------------------------------------------------------------------
void returnFail(String msg)
{
    webServer.send(500, FPSTR(kTEXTPLAIN), msg + "\r\n");
}

//---------------------------------------------------------------------------------
void respondOK()
{
    webServer.send(200, FPSTR(kTEXTPLAIN), "OK");
}

//---------------------------------------------------------------------------------
void handle_update()
{
    webServer.sendHeader("Connection", "close");
    webServer.sendHeader(FPSTR(kACCESSCTL), "*");
    webServer.send(200, FPSTR(kTEXTHTML), FPSTR(kUPLOADFORM));
}

//---------------------------------------------------------------------------------
void handle_upload()
{
    webServer.sendHeader("Connection", "close");
    webServer.sendHeader(FPSTR(kACCESSCTL), "*");
    webServer.send(200, FPSTR(kTEXTPLAIN), (Update.hasError()) ? "FAIL" : "OK");
    if (updateCB)
    {
        updateCB->updateCompleted();
    }
    ESP.restart();
}

//---------------------------------------------------------------------------------
void handle_upload_status()
{
    bool success = true;
    if (!started)
    {
        started = true;
        if (updateCB)
        {
            updateCB->updateStarted();
        }
    }
    HTTPUpload &upload = webServer.upload();
    if (upload.status == UPLOAD_FILE_START)
    {
#ifdef DEBUG_SERIAL
        DEBUG_SERIAL.setDebugOutput(true);
#endif
        WiFiUDP::stopAll();
#ifdef DEBUG_SERIAL
        DEBUG_SERIAL.printf("Update: %s\n", upload.filename.c_str());
#endif
        uint32_t maxSketchSpace = (ESP.getFreeSketchSpace() - 0x1000) & 0xFFFFF000;
        if (!Update.begin(maxSketchSpace))
        {
#ifdef DEBUG_SERIAL
            Update.printError(DEBUG_SERIAL);
#endif
            success = false;
        }
    }
    else if (upload.status == UPLOAD_FILE_WRITE)
    {
        if (Update.write(upload.buf, upload.currentSize) != upload.currentSize)
        {
#ifdef DEBUG_SERIAL
            Update.printError(DEBUG_SERIAL);
#endif
            success = false;
        }
    }
    else if (upload.status == UPLOAD_FILE_END)
    {
        if (Update.end(true))
        {
#ifdef DEBUG_SERIAL
            DEBUG_SERIAL.printf("Update Success: %u\nRebooting...\n", upload.totalSize);
#endif
        }
        else
        {
#ifdef DEBUG_SERIAL
            Update.printError(DEBUG_SERIAL);
#endif
            success = false;
        }
#ifdef DEBUG_SERIAL
        DEBUG_SERIAL.setDebugOutput(false);
#endif
    }
    yield();
    if (!success)
    {
        if (updateCB)
        {
            updateCB->updateError();
        }
    }
}

//---------------------------------------------------------------------------------
void handle_getParameters()
{
    String message = FPSTR(kHEADER);
    message += "<div class=topnav id=BRtopnav><a href=/getparameters class=active>Parameters</a> <a href=/setup >Setup</a> <a href=/getstatus>Status</a> <a href=/update>Firmware Update</a> <a href=/reboot>Reboot</a> <a href=javascript:void(0); class=icon onclick=changeTopnav()><div class=container onclick=hamburgerMenu(this)><div class=bar1></div><div class=bar2></div><div class=bar3></div></div></a></div>";
    message += "<p>Parameters</p><table><tr><td width=\"240\">Name</td><td>Value</td></tr>";
    for (int i = 0; i < MavESP8266Parameters::ID_COUNT; i++)
    {
        message += "<tr><td>";
        message += getWorld()->getParameters()->getAt(i)->id;
        message += "</td>";
        unsigned long val = 0;
        if (getWorld()->getParameters()->getAt(i)->type == MAV_PARAM_TYPE_UINT32)
            val = (unsigned long)*((uint32_t *)getWorld()->getParameters()->getAt(i)->value);
        else if (getWorld()->getParameters()->getAt(i)->type == MAV_PARAM_TYPE_UINT16)
            val = (unsigned long)*((uint16_t *)getWorld()->getParameters()->getAt(i)->value);
        else
            val = (unsigned long)*((int8_t *)getWorld()->getParameters()->getAt(i)->value);
        message += "<td>";
        message += val;
        message += "</td></tr>";
    }
    message += "</table>";
    message += "</body>";
    webServer.send(200, FPSTR(kTEXTHTML), message);
}

//---------------------------------------------------------------------------------
static void handle_root()
{
    String message = FPSTR(kHEADER);
    message += "<p id=\"title\">Version: ";
    char vstr[30];
    snprintf(vstr, sizeof(vstr), "%u.%u.%u", MAVESP8266_VERSION_MAJOR, MAVESP8266_VERSION_MINOR, MAVESP8266_VERSION_BUILD);
    message += vstr;
    message += "</p>\n";
    message += "<p>\n";
    message += "<ul>\n";
    message += "<li><a href='/getstatus'>Get Status</a>\n";
    message += "<li><a href='/setup'>Setup</a>\n";
    message += "<li><a href='/getparameters'>Get Parameters</a>\n";
    message += "<li><a href='/update'>Update Firmware</a>\n";
    message += "<li><a href='/reboot'>Reboot</a>\n";
    message += "</ul></body>";
    setNoCacheHeaders();
    webServer.send(200, FPSTR(kTEXTHTML), message);
}

//---------------------------------------------------------------------------------
static void handle_setup()
{
    String message = FPSTR(kHEADER);
    message += "<div class=topnav id=BRtopnav><a href=/setup class=active>Setup</a> <a href=/getstatus>Status</a> <a href=/getparameters>Parameters</a> <a href=/update>Firmware Update</a> <a href=/reboot>Reboot</a> <a href=javascript:void(0); class=icon onclick=changeTopnav()><div class=container onclick=hamburgerMenu(this)><div class=bar1></div><div class=bar2></div><div class=bar3></div></div></a></div>";
    message += "<h1>Setup</h1>\n";
    message += "<form action='/setparameters' method='post'>\n";

    message += "WiFi Mode:&nbsp;";
    message += "<input type='radio' name='mode' value='0'";
    if (getWorld()->getParameters()->getWifiMode() == WIFI_MODE_AP)
    {
        message += " checked";
    }
    message += ">AccessPoint\n";
    message += "<input type='radio' name='mode' value='1'";
    if (getWorld()->getParameters()->getWifiMode() == WIFI_MODE_STA)
    {
        message += " checked";
    }
    message += ">Station<br>\n";

    message += "AP SSID:&nbsp;";
    message += "<input type='text' name='ssid' value='";
    message += getWorld()->getParameters()->getWifiSsid();
    message += "'><br>";

    message += "AP Password (min len 8):&nbsp;";
    message += "<input type='text' name='pwd' value='";
    message += getWorld()->getParameters()->getWifiPassword();
    message += "'><br>";

    message += "WiFi Channel:&nbsp;";
    message += "<input type='text' name='channel' value='";
    message += getWorld()->getParameters()->getWifiChannel();
    message += "'><br>";

    message += "Station SSID:&nbsp;";
    message += "<input type='text' name='ssidsta' value='";
    message += getWorld()->getParameters()->getWifiStaSsid();
    message += "'><br>";

    message += "Station Password:&nbsp;";
    message += "<input type='text' name='pwdsta' value='";
    message += getWorld()->getParameters()->getWifiStaPassword();
    message += "'><br>";

    IPAddress IP;
    message += "Station IP:&nbsp;";
    message += "<input type='text' name='ipsta' value='";
    IP = getWorld()->getParameters()->getWifiStaIP();
    message += IP.toString();
    message += "'><br>";

    message += "Station Gateway:&nbsp;";
    message += "<input type='text' name='gatewaysta' value='";
    IP = getWorld()->getParameters()->getWifiStaGateway();
    message += IP.toString();
    message += "'><br>";

    message += "Station Subnet:&nbsp;";
    message += "<input type='text' name='subnetsta' value='";
    IP = getWorld()->getParameters()->getWifiStaSubnet();
    message += IP.toString();
    message += "'><br>";

    message += "Host Port:&nbsp;";
    message += "<input type='text' name='hport' value='";
    message += getWorld()->getParameters()->getWifiUdpHport();
    message += "'><br>";

    message += "Client Port:&nbsp;";
    message += "<input type='text' name='cport' value='";
    message += getWorld()->getParameters()->getWifiUdpCport();
    message += "'><br>";

    message += "Baudrate:&nbsp;";
    message += "<input type='text' name='baud' value='";
    message += getWorld()->getParameters()->getUartBaudRate();
    message += "'><br>";

    message += "<input type='submit' value='Save'>";
    message += "</form>";
    setNoCacheHeaders();
    webServer.send(200, FPSTR(kTEXTHTML), message);
}

//---------------------------------------------------------------------------------
static void handle_getStatus()
{
    if (!flash)
        flash = ESP.getFreeSketchSpace();
    if (!paramCRC[0])
    {
        snprintf(paramCRC, sizeof(paramCRC), "%08X", getWorld()->getParameters()->paramHashCheck());
    }
    linkStatus *gcsStatus = getWorld()->getGCS()->getStatus();
    linkStatus *vehicleStatus = getWorld()->getVehicle()->getStatus();
    String message = FPSTR(kHEADER);
    message += "<p>Comm Status</p><table><tr><td width=\"240\">Packets Received from GCS</td><td>";
    message += gcsStatus->packets_received;
    message += "</td></tr><tr><td>Packets Sent to GCS</td><td>";
    message += gcsStatus->packets_sent;
    message += "</td></tr><tr><td>GCS Packets Lost</td><td>";
    message += gcsStatus->packets_lost;
    message += "</td></tr><tr><td>Packets Received from Vehicle</td><td>";
    message += vehicleStatus->packets_received;
    message += "</td></tr><tr><td>Packets Sent to Vehicle</td><td>";
    message += vehicleStatus->packets_sent;
    message += "</td></tr><tr><td>Vehicle Packets Lost</td><td>";
    message += vehicleStatus->packets_lost;
    message += "</td></tr><tr><td>Radio Messages</td><td>";
    message += gcsStatus->radio_status_sent;
    message += "</td></tr></table>";
    message += "<p>System Status</p><table>\n";
    message += "<tr><td width=\"240\">Flash Size</td><td>";
    message += ESP.getFlashChipRealSize();
    message += "</td></tr>\n";
    message += "<tr><td width=\"240\">Flash Available</td><td>";
    message += flash;
    message += "</td></tr>\n";
    message += "<tr><td>RAM Left</td><td>";
    message += String(ESP.getFreeHeap());
    message += "</td></tr>\n";
    message += "<tr><td>Parameters CRC</td><td>";
    message += paramCRC;
    message += "</td></tr>\n";
    message += "</table>";
    message += "</body>";
    setNoCacheHeaders();
    webServer.send(200, FPSTR(kTEXTHTML), message);
}

//---------------------------------------------------------------------------------
void handle_getJLog()
{
    uint32_t position = 0, len;
    if (webServer.hasArg(kPOSITION))
    {
        position = webServer.arg(kPOSITION).toInt();
    }
    String logText = getWorld()->getLogger()->getLog(&position, &len);
    char jStart[128];
    snprintf(jStart, 128, "{\"len\":%d, \"start\":%d, \"text\": \"", len, position);
    String payLoad = jStart;
    payLoad += logText;
    payLoad += "\"}";
    webServer.send(200, FPSTR(kAPPJSON), payLoad);
}

//---------------------------------------------------------------------------------
void handle_getJSysInfo()
{
    if (!flash)
        flash = ESP.getFreeSketchSpace();
    if (!paramCRC[0])
    {
        snprintf(paramCRC, sizeof(paramCRC), "%08X", getWorld()->getParameters()->paramHashCheck());
    }
    uint32_t fid = spi_flash_get_id();
    char message[512];
    snprintf(message, 512,
             "{ "
             "\"size\": \"%s\", "
             "\"id\": \"0x%02lX 0x%04lX\", "
             "\"flashfree\": \"%u\", "
             "\"heapfree\": \"%u\", "
             "\"logsize\": \"%u\", "
             "\"paramcrc\": \"%s\""
             " }",
             kFlashMaps[system_get_flash_size_map()],
             (long unsigned int)(fid & 0xff), (long unsigned int)((fid & 0xff00) | ((fid >> 16) & 0xff)),
             flash,
             ESP.getFreeHeap(),
             getWorld()->getLogger()->getPosition(),
             paramCRC);
    webServer.send(200, "application/json", message);
}

//---------------------------------------------------------------------------------
void handle_getJSysStatus()
{
    bool reset = false;
    if (webServer.hasArg("r"))
    {
        reset = webServer.arg("r").toInt() != 0;
    }
    linkStatus *gcsStatus = getWorld()->getGCS()->getStatus();
    linkStatus *vehicleStatus = getWorld()->getVehicle()->getStatus();
    if (reset)
    {
        memset(gcsStatus, 0, sizeof(linkStatus));
        memset(vehicleStatus, 0, sizeof(linkStatus));
    }
    char message[512];
    snprintf(message, 512,
             "{ "
             "\"gpackets\": \"%u\", "
             "\"gsent\": \"%u\", "
             "\"glost\": \"%u\", "
             "\"vpackets\": \"%u\", "
             "\"vsent\": \"%u\", "
             "\"vlost\": \"%u\", "
             "\"radio\": \"%u\", "
             "\"buffer\": \"%u\""
             " }",
             gcsStatus->packets_received,
             gcsStatus->packets_sent,
             gcsStatus->packets_lost,
             vehicleStatus->packets_received,
             vehicleStatus->packets_sent,
             vehicleStatus->packets_lost,
             gcsStatus->radio_status_sent,
             vehicleStatus->queue_status);
    webServer.send(200, "application/json", message);
}

//---------------------------------------------------------------------------------
void handle_setParameters()
{
    if (webServer.args() == 0)
    {
        returnFail(kBADARG);
        return;
    }
    bool ok = false;
    bool reboot = false;
    if (webServer.hasArg(kBAUD))
    {
        ok = true;
        getWorld()->getParameters()->setUartBaudRate(webServer.arg(kBAUD).toInt());
    }
    if (webServer.hasArg(kPWD))
    {
        // Ensure password is between 8 characters and 16 characters
        if (webServer.arg(kPWD).length() >= 8 && webServer.arg(kPWD).length() <= 16)
        {
            ok = true;
            getWorld()->getParameters()->setWifiPassword(webServer.arg(kPWD).c_str());
        }
    }
    if (webServer.hasArg(kSSID))
    {
        // Ensure SSID is less than or equal to 16 characters
        if (webServer.arg(kSSID).length() <= 16)
        {
            ok = true;
            getWorld()->getParameters()->setWifiSsid(webServer.arg(kSSID).c_str());
        }
    }
    if (webServer.hasArg(kPWDSTA))
    {
        ok = true;
        getWorld()->getParameters()->setWifiStaPassword(webServer.arg(kPWDSTA).c_str());
    }
    if (webServer.hasArg(kSSIDSTA))
    {
        ok = true;
        getWorld()->getParameters()->setWifiStaSsid(webServer.arg(kSSIDSTA).c_str());
    }
    if (webServer.hasArg(kIPSTA))
    {
        IPAddress ip;
        ip.fromString(webServer.arg(kIPSTA).c_str());
        getWorld()->getParameters()->setWifiStaIP(ip);
    }
    if (webServer.hasArg(kGATESTA))
    {
        IPAddress ip;
        ip.fromString(webServer.arg(kGATESTA).c_str());
        getWorld()->getParameters()->setWifiStaGateway(ip);
    }
    if (webServer.hasArg(kSUBSTA))
    {
        IPAddress ip;
        ip.fromString(webServer.arg(kSUBSTA).c_str());
        getWorld()->getParameters()->setWifiStaSubnet(ip);
    }
    if (webServer.hasArg(kCPORT))
    {
        ok = true;
        getWorld()->getParameters()->setWifiUdpCport(webServer.arg(kCPORT).toInt());
    }
    if (webServer.hasArg(kHPORT))
    {
        ok = true;
        getWorld()->getParameters()->setWifiUdpHport(webServer.arg(kHPORT).toInt());
    }
    if (webServer.hasArg(kCHANNEL))
    {
        ok = true;
        getWorld()->getParameters()->setWifiChannel(webServer.arg(kCHANNEL).toInt());
    }
    if (webServer.hasArg(kDEBUG))
    {
        ok = true;
        getWorld()->getParameters()->setDebugEnabled(webServer.arg(kDEBUG).toInt());
    }
    if (webServer.hasArg(kMODE))
    {
        ok = true;
        getWorld()->getParameters()->setWifiMode(webServer.arg(kMODE).toInt());
    }
    if (webServer.hasArg(kREBOOT))
    {
        ok = true;
        reboot = webServer.arg(kREBOOT) == "1";
    }
    if (ok)
    {
        getWorld()->getParameters()->saveAllToEeprom();
        //-- Send new parameters back
        handle_getParameters();
        if (reboot)
        {
            delay(100);
            ESP.restart();
        }
    }
    else
        returnFail(kBADARG);
}

//---------------------------------------------------------------------------------
static void handle_reboot()
{
    String message = FPSTR(kHEADER);
    message += "rebooting ...</body>\n";
    setNoCacheHeaders();
    webServer.send(200, FPSTR(kTEXTHTML), message);
    delay(500);
    ESP.restart();
}

//---------------------------------------------------------------------------------
//-- 404
void handle_notFound()
{
    String message = "File Not Found\n\n";
    message += "URI: ";
    message += webServer.uri();
    message += "\nMethod: ";
    message += (webServer.method() == HTTP_GET) ? "GET" : "POST";
    message += "\nArguments: ";
    message += webServer.args();
    message += "\n";
    for (uint8_t i = 0; i < webServer.args(); i++)
    {
        message += " " + webServer.argName(i) + ": " + webServer.arg(i) + "\n";
    }
    webServer.send(404, FPSTR(kTEXTPLAIN), message);
}

//---------------------------------------------------------------------------------
MavESP8266Httpd::MavESP8266Httpd()
{
}

//---------------------------------------------------------------------------------
//-- Initialize
void MavESP8266Httpd::begin(MavESP8266Update *updateCB_)
{
    updateCB = updateCB_;
    webServer.on("/", handle_root);
    webServer.on("/getparameters", handle_getParameters);
    webServer.on("/setparameters", handle_setParameters);
    webServer.on("/getstatus", handle_getStatus);
    webServer.on("/reboot", handle_reboot);
    webServer.on("/setup", handle_setup);
    webServer.on("/info.json", handle_getJSysInfo);
    webServer.on("/status.json", handle_getJSysStatus);
    webServer.on("/log.json", handle_getJLog);
    webServer.on("/update", handle_update);
    webServer.on("/upload", HTTP_POST, handle_upload, handle_upload_status);
    webServer.onNotFound(handle_notFound);
    webServer.begin();
}

//---------------------------------------------------------------------------------
//-- Initialize
void MavESP8266Httpd::checkUpdates()
{
    webServer.handleClient();
}
