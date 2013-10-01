/*

  # Connection Test
  
  Basic example that shows how to stream video to a Flash Media Server or alike, 
  using the `TestPattern` generator. This `TestPattern` generates a simple red/yellow,
  blue checkerboard pattern and a simple sine wave audio stream.

  See the connection_test.xml file in the install directory where you can set the 
  url of the media server and widht/height etc.. 

 */
#include <signal.h>
#include <iostream>
#include <string>
#include <tinylib/tinylib.h>
#include <streamer/videostreamer/VideoStreamer.h>
#include <streamer/core/TestPattern.h>

bool must_run = false;

void sighandler(int signum);

int main() {

  VideoStreamer vs;
  
  std::string settings_file = rx_get_exe_path() +"connection_test.xml";
  if(!vs.loadSettings(settings_file)) {
    printf("error: cannot find the connection_test.xml file: %s. \n", settings_file.c_str());
    ::exit(EXIT_FAILURE);
  }

  TestPattern tp;
  if(!tp.setup(vs.getVideoWidth(), vs.getVideoHeight(), vs.getFrameRate(), vs.getSampleRate())) {
    printf("error: cannot setup the test pattern.\n");
    ::exit(EXIT_FAILURE);
  }

  printf("Loaded streamer with: %d x %d @ %d, samplerate: %d\n", vs.getVideoWidth(), vs.getVideoHeight(), vs.getFrameRate(), vs.getSampleRate());

  signal(SIGINT, sighandler);

  if(!vs.setup()) {
    ::exit(EXIT_FAILURE);
  }

  if(!vs.start()) {
    ::exit(EXIT_FAILURE);
  }

  tp.start();

  int nbytes_video = vs.getVideoWidth() * vs.getVideoHeight() + (2 * (vs.getVideoWidth() * 0.5) * (vs.getVideoHeight() * 0.5));

  must_run = true;

  while(must_run) {

    tp.update();

    if(tp.hasVideoFrame()) {
      AVPacket* vid_pkt = new AVPacket();
      vid_pkt->allocate(nbytes_video);
      tp.generateVideoFrame(vid_pkt->data);
      vid_pkt->makeVideoPacket();
      vid_pkt->setTimeStamp(tp.timestamp);
      vs.addVideo(vid_pkt);
    }
    
    if(tp.hasAudioFrame()) {
      //("YES audio.\n");
    }

  }

  return EXIT_SUCCESS;
}


void sighandler(int signum) {
  printf("\nStop!\n");
  must_run = false;
}
