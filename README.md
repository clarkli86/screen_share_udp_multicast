# Screen Share in Qt

## Purpose
This was the project that I did for Linux programming back to 10 years
ago using Qt. It now has been updated from Qt3 to Qt5...
Since it demonstrates UDP and multicasting, I decided to open source it
as an example.

## Main Features
* Capture the screenshot on server
* Send screnshots to multiple clients using UDP multicast

## How it works
### Server
* Server creates a multicast address with the first three IPv4 segments + the
  last segment of server's own IPv4. E.g, the multicast address is 224.0.0.2
  if server's address is 192.168.1.2.
* All servers in the same network broadcast their multicast addresses to 224.0.0.1:5000
* Each server also contuously sends screenshots to their own multicast addresses. E.g.
  224.0.0.2:5000
### client
* Client retrieves a list of server multicast addresses from 224.0.0.1:5000
* If a server is selected, client joins the multicast group to receive the
  screenshots from the selected server

