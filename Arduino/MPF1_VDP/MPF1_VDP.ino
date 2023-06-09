#include <TVout.h>
#include <fontALL.h>

#define BUFFER_SIZE 20
#define RX_WAITING_BYTE        0
#define RX_WAITING_HIGH_NIBBLE 1
#define RX_WAITING_LOW_NIBBLE  2
#define CMD_CURSOR_POS  1
#define CMD_FONT        2
#define CMD_LINE        3
#define CMD_RECT        4
#define CMD_RECTF       5
#define CMD_CIRCLE      6
#define CMD_CIRCLEF     7
#define CMD_TEXT_MODE   8
#define CMD_CURSOR_MODE 9
#define CMD_MIRROR_MODE 10


TVout TV;

unsigned volatile int buffer[BUFFER_SIZE], front = -1, rear = -1, state, hi;
unsigned int arguments[4], argument_index = 0, command_mode = 0, command = 0;

PROGMEM const unsigned char MPFLogo[] = {96, 71, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xdf, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfc, 0x01, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xc0, 0x00, 0x7f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfe, 0x00, 0x00, 0x01, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf0, 0x00, 0x00, 0x00, 0x3f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x07, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3f, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0xff, 0xff, 0xff, 0xff, 0xf8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7f, 0xff, 0xff, 0xff, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0xff, 0xff, 0xf8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0f, 0xfc, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0xf3, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7f, 0xf3, 0xfc, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0xff, 0xf3, 0xff, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xf3, 0xff, 0xfc, 0x00, 0x00, 0x00, 0x0f, 0x80, 0x00, 0x03, 0xff, 0xff, 0xf3, 0xff, 0xff, 0xc0, 0x00, 0x00, 0x09, 0xc0, 0x00, 0x3f, 0xff, 0xff, 0xf3, 0xff, 0xff, 0xfc, 0x00, 0x00, 0x08, 0xc0, 0x01, 0xff, 0xff, 0xff, 0xf3, 0xff, 0xff, 0xfc, 0x00, 0x00, 0x08, 0xc0, 0x03, 0xff, 0xff, 0xff, 0xe3, 0xff, 0xff, 0xfe, 0x00, 0x00, 0x08, 0xc0, 0x03, 0xff, 0xff, 0xff, 0xe3, 0xff, 0xff, 0xfc, 0x00, 0x00, 0x0f, 0xc0, 0x03, 0xff, 0xff, 0xff, 0xe3, 0xff, 0xff, 0xfc, 0x00, 0x31, 0x8f, 0x00, 0x03, 0xff, 0xff, 0xff, 0xe3, 0xff, 0xff, 0xfc, 0x00, 0x31, 0x8c, 0x00, 0x03, 0xff, 0xff, 0xff, 0xe3, 0xff, 0xff, 0xfc, 0x00, 0x31, 0x8c, 0x00, 0x01, 0xff, 0xff, 0xff, 0xe3, 0xff, 0xff, 0xfc, 0x00, 0x33, 0x8c, 0x00, 0x01, 0xff, 0xff, 0xff, 0xe3, 0xff, 0xff, 0xfc, 0x00, 0x3f, 0x8c, 0x00, 0x01, 0xff, 0xff, 0xff, 0xe3, 0xff, 0xff, 0xf8, 0x00, 0x20, 0xcc, 0x00, 0x01, 0xff, 0xff, 0xff, 0xe3, 0xff, 0xff, 0xf8, 0x00, 0x60, 0x08, 0x00, 0x01, 0xff, 0xff, 0xff, 0xe3, 0xff, 0xff, 0xfc, 0x00, 0x40, 0x00, 0x00, 0x01, 0xff, 0xff, 0xff, 0xe3, 0xff, 0xff, 0xfe, 0x00, 0x00, 0x00, 0x00, 0x07, 0xff, 0xff, 0xff, 0xe3, 0xff, 0xff, 0xff, 0xe0, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 0xe3, 0xff, 0xff, 0xff, 0xfc, 0x00, 0x00, 0x03, 0xff, 0xff, 0xff, 0xff, 0xe3, 0xff, 0xff, 0xff, 0xff, 0x80, 0x00, 0x1f, 0xff, 0xff, 0xff, 0xff, 0xe3, 0xff, 0xff, 0xff, 0xff, 0xf0, 0x01, 0xff, 0xff, 0xff, 0xff, 0xff, 0xe3, 0xff, 0xff, 0xff, 0xff, 0xfe, 0x07, 0xff, 0xff, 0xff, 0xff, 0xff, 0xc1, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xc1, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xc1, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xc1, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xc1, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xc1, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xc1, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xc1, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xc1, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xc1, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xc1, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xc1, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xc1, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xc1, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xc1, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xc1, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xc1, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x81, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x81, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x81, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x81, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x81, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x81, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x81, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x81, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x81, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x81, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x81, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x81, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x81, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x81, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x81, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff};

void en_queue(unsigned int value) {
  if (rear == BUFFER_SIZE - 1) {
    return;
  } else {
    if (front == -1)
      front = 0;
    rear++;
    buffer[rear] = value;
  }
}

unsigned int de_queue() {
  unsigned int b = buffer[front];
  front++;
  if (front > rear)
    front = rear = -1;
  return b;
}

unsigned int empty_queue() {
  return (front == -1);
}

void data_ready() {
  unsigned int nib = PINC;
  switch (state) {
    case RX_WAITING_BYTE:
      if (nib == 0xF) state = RX_WAITING_HIGH_NIBBLE;
      break;
    case RX_WAITING_HIGH_NIBBLE:
      hi = nib;
      state = RX_WAITING_LOW_NIBBLE;
      break;
    case RX_WAITING_LOW_NIBBLE:
      unsigned int val = (hi << 4) | nib;
      en_queue(val);
      state = RX_WAITING_BYTE;
      break;
  }
}

void setup() {
  TV.begin(PAL);  //for devices with only 1k sram(m168) use TV.begin(_NTSC,128,56)
  TV.select_font(font6x8);
  TV.clear_screen();
  DDRC = 0;  // all port C as input
  attachInterrupt(digitalPinToInterrupt(3), data_ready, RISING);
  TV.bitmap((TV.hres() - 92) / 2, 10, MPFLogo);
  TV.delay(2000);
  TV.clear_screen();
}

void clear_command() {
  command_mode = 0;
  command = 0;
  argument_index = 0;
}

void do_command(unsigned int b) {
  switch (command) {
    case CMD_CURSOR_POS:
      if (argument_index < 1) {
        arguments[argument_index++] = b;
      } else {
        TV.set_char_cursor(arguments[0], b);
        clear_command();
      }
      break;
    case CMD_FONT:
      switch (b) {
        case 1:
          TV.select_font(font4x6);
          break;
        case 2:
          TV.select_font(font6x8);
          break;
        case 3:
          TV.select_font(font8x8);
          break;
        case 4:
          TV.select_font(font8x8ext);
          break;
        case 5:
          TV.select_font(bescii8x8);
          break;
        default:
          TV.select_font(font6x8);
          break;
      }
      clear_command();
      break;
    case CMD_LINE:
      if (argument_index < 3) {
        arguments[argument_index] = b;
        argument_index++;
      } else {
        TV.draw_line(arguments[0], arguments[1], arguments[2], b, 1);
        clear_command();
      }
      break;
    case CMD_RECT:
    case CMD_RECTF:
      if (argument_index < 3) {
        arguments[argument_index++] = b;
      } else {
        TV.draw_rect(arguments[0], arguments[1], arguments[2], b, 1, (command == CMD_RECT) ? -1 : 1);
        clear_command();
      }
      break;
    case CMD_CIRCLE:
    case CMD_CIRCLEF:
      if (argument_index < 2) {
        arguments[argument_index++] = b;
      } else {
        TV.draw_circle(arguments[0], arguments[1], b, 1, (command == CMD_CIRCLE) ? -1 : 1);
        clear_command();
      }
      break;
    case CMD_TEXT_MODE:
      TV.set_invert((b == 1) ? 0 : 1);
      clear_command();
      break;
    case CMD_CURSOR_MODE:
      TV.enable_cursor((b == 1) ? 0 : 1);
      clear_command();
      break;
    case CMD_MIRROR_MODE:
      TV.set_mirror((b == 1) ? 0 : 1);
      clear_command();
      break;
    default:
      clear_command();
  }
}

void loop() {
  if (!empty_queue()) {
    unsigned int b = de_queue();
    if (command_mode) {
      if (command == 0) {
        command = b;
      } else {
        do_command(b);
      }
    } else {
      switch (b) {
        case 0xC:
          TV.clear_screen();
          break;
        case 0xA:
          TV.println();
          break;
        case 0x1B:
          command_mode = 1;
          break;
        default:
          TV.write(b);
      }
    }
  }
}
