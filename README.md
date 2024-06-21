# reverseDelay-Help
a "working" reverse delay dsp.

problems:
  - no DAW clock sync
  - feedback is written to circular buffer in reverse. since the buffer is reading the reverse of the input, writing anything in reverse will be read as forward.
  - user interface is... there

OTHER than that, this dsp works. I haven't seen any simple reverse delays out in the wild so I hope this is found useful to someone. 

socials/music:
@archosis. on IG
------------------
archosis.bandcamp.com
