dcn_webserver
=============

In this lab you will develop a limited Web server that can detect some Request errors, process multiple requests simultaneously and support caching. You should be able to demonstrate that your Web server is capable of delivering content to a Web browser.


First, I would like to emphasize that I do this web server programming on the Linux System (Ubuntu Linux 11.10). So the best way to test my web server program is running it on the Linux System.
Second, under the "WebServer" directory, using "make" command to compile and link the web server program.
Third, under the "TestClient" directory, using "make" command to compile and link the test client program.
Fourth, "TestClient.exe" is just a tool that could automate test the web server without network.
Fifth, Write the HTTP Request Message in "Request_Message.txt" file which is in the "TestClient" document, and run "WebServer.ext" and "TestClient.exe". The HTTP Response Message will write to "Response_Message.txt" in "TestClient" document automatically.
Sixth, you could write any HTTP Request Message if you want. Note that each HTTP Request Message should separate by a CRLF line (a blank line with only CRLF).

That's all, and enjoy my project.
