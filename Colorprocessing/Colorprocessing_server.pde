// 2A: Shared drawing canvas (Server)

import processing.net.*;

Server s;
Client c;
String input;
int data[];

void setup() {
  size(450, 255);
  background(204);
  stroke(0);
  frameRate(5); // Slow it down a little
  s = new Server(this, 8080);  // Start a simple server on a port
}
void draw() {
  // Receive data from client
  c = s.available();
  if (c != null) {
    input = c.readString();
    print(input);
  }
}