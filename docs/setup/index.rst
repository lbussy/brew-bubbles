Setting up the Environment
==========================

Once you have flashed the firmware and spiffs files to your controller, it will restart in Access Point mode with a captive portal.   A captive portal is similar to what you get when you connect to free wifi with a login web page.  Any network communication will be redirected to the portal page, allowing you to login.  This is how Brew Bubbles is initially configured, allowing it to connect to your local wifi.

This process works best through a phone in most cases.  There are some peculiarities in the ESP8266 libraries which cause the process to sometimes act a little flakey, not open the portal page, even not be issued an IP address.  In testing, these problems did not come up using a phone.  If you don;t have a phone handy it is possible to do this work with your computer, there are some caveats which will be pointed out below.

Blink Modes
-----------

The LED on the ESP8266 is an indicator of various modes during operation.  When in AP mode, the LED will blink on and off at 0.5Hz.  That is, it will be on for a second and off for a second.  When in this mode, the access point will show up in the list of available access points.

Connect to Captive Portal
=========================

Connect to the access point as follows:

Login:
    Access Point Name (SSID): brewbubbles
    Password: brewbubbles

.. figure:: 1_select-wifi.png
   :scale: 90 %
   :align: center
   :alt: List of available access points

See documentation for your particular phone, device, or computer OS if you don't know how to access this list.

.. figure:: 2_password.png
   :scale: 90 %
   :align: center
   :alt: Enter password for Brew Bubbles access point

Select the Brew Bubbles access point and enter the password: "brewbubbles" (without the quotes:)

.. figure:: 3_wifi_selected.jpg
   :scale: 90 %
   :align: center
   :alt: Connected to soft AP

Once you are connected, and depending on the platform upon which you are working, a web page should open displaying the portal.  With iOS this should happen automatically.  On various PCs during testing this was not 100%.  Two issues were noted:

Captive Portal Page does not Open
`````````````````````````````````

#. If your default web browser does not open to the portal page automatically, open the web browser of your choice.  It is likely at this point the captive portal will be displayed.
#. If the portal does not automatially open, a bar may appear at the top of your browser with a button having a notice that the network login page needs to be opened.  Clicking this button should open the portal.
#. If the portal still does not open, enter the address "192.168.4.1" (without quotes) in the address bar.  This is the address assigned to the Brew Bubbles controller in this mode.

Connection Timed Out or Similar Errors

#. If you cannot open the captive portal web page by following the above instructions, it is possible that your device was not issued an IP address.  Follow instructions for your particular platform to set the following parameters:

IP Configuration:
    IP Address:  192.168.4.2
    Subnet Mask: 255.255.255.0
    Gateway: 192.168.4.1

These methods should allow you to configure the portal.  If you cannot connect to the portal, please try another sytem/method before logging an issue.

Portal Configuration
====================

When you access the captive portal you will be presented with six choices:

#. Configure WiFi
#. Configure WiFi (No Scan)
#. Info
#. Erase
#. Restart
#. Exit

.. figure:: 4_captive_portal.jpg
   :scale: 90 %
   :align: center
   :alt: Captive portal choices

Select option 1, "Configure WiFi."  You will be presented with the following configuration screen:

.. figure:: 5_ select_ap.jpg
   :scale: 90 %
   :align: center
   :alt: Captive portal choices

Your access point should be visible in the list on top.  Selecting it will populate the name in the SSID field below.  If for some reason your access point does not show up, you can try to re-scan, or simply enter the AP name manually.  Note this name is case-sensitive.  Next, enter your access point password in the "Password" field.

If you need to use a static IP address, you must fill out the following fields:

#. Static IP
#. Static Gateway
#. Subnet
#. Static DNS

If you have questions about thiese fields, consult the documentation for your access point.  You need not fill out these fields to use an automatically assigned IP address, since you may access the device by it's name.

Once you have filled out at least the SSID and Password, click on the "Save" button.

.. figure:: 6_save_ap.jpg
   :scale: 90 %
   :align: center
   :alt: Save WiFi configuration

