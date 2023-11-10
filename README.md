# RoseTransmitter
Remote control for the rose in Beauty and the Beast; based on ESP8266.

This project is a companion to [RoseController project](https://github.com/tomikazi/RoseController)
and serves a dedicated harware remote control for the rose in lieu of a smart-phone or a similar mobile device.

The device provides three buttons (and corresponding LED indicators) to remotely toggle the 
three lights (Spot/Dome/Pixie) and a fourth button to sequentially actuate 5 servos used to drop the rose petals.

The transmitter automatically joins the RoseController WiFi and uses its REST API to send commands and to receive status.
Upon losing the connection, it automatically attempts to reconnect.

The software is built atop the [ESPGizmo platform library](https://github.com/tomikazi/ESPGizmo).
