/*
   Music Sequencer for Arduino/Motor System
   (c) Masato ISHIKAWA 2012-2022
*/

/*************************************************
  MML(Music Macro Language) Syntax
  (1) notes
     C,D,E,F,G,A,B, R  : notes(A-G) and pause(R)
     +, -              : sharp, flat
     4, 8, 2, 16, etc. : note length
     .                 : length * 1.5
     &                 : tie

  (2) commands
      Tnnn   : tempo (default=T120)
      Lnn    : set context note length (default=L4)
      On     : set octave (2<=n<=6, default=O4)
      <      : octave up
      >      : octave down
***************************************************/

/* モータまたはスピーカをつなぐ 
　 下記の曲は2重和音なので，D7とD8に モータまたはパッシブスピーカをつなぎます．
　　モータの場合：電流はほとんど流れないので直付けでOK（モータドライバ不要）．紙コップなどで拡声を
　　パッシブスピーカの場合：直付けでOK，かなりうるさいです */
#define INSTS 2
int motor[INSTS] = {7, 8};


/******* write your score here *******************/
/* メモリを節約するために，曲データはフラッシュメモリの方に置くという技を使っています */
#define score_MAX 1000

// Jingle Bell
const char score_list[INSTS][score_MAX] PROGMEM = {
  "T80L4O4 B8B8B B8B8B B8<D8>G8.A16B R <C8C8C8.C16C8>B8B8B16<C16> B8A8A8B8A <D> B8B8B B8B8B B8<D8>G8.A16B R <C8C8C8.C16C8>B8B8B16B16 <D8D8C8>A8G. R1",
  "T80L4O4 G8G8G G8G8G G8G8D8.E16G   R E8E8F8.E16E8D8D8G16G16     G8G8G8G8F+ F+ G8G8G G8G8G G8G8D8.E16G   R E8E8F8.E16E8D8D8G16G16   F+8F+8F+8F+8G. R1 "
};
#define score(n,i) (pgm_read_byte( &(score_list[n][i])))
/**************************************************/

/* a tenth of period in usec (duty ratio = 5%) */
#define Hz2usec 50000
#define halfstep 1.05946
/* Bottom note in Hz */
#define C2 65.408
#define PAUSE 9999

int semitones[60]; /* all semitones from C2-B6 */
const int diatonic [] = {9, 11, 0, 2, 4, 5, 7};

/* temporal control */
int dacapo[INSTS]; /* end of music */
int tick[INSTS];
int duration[INSTS];
int period[INSTS];
boolean state[INSTS]; 
unsigned long laptime[INSTS];
unsigned long nextime[INSTS];

#define notes_MAX 250
unsigned short int notes[INSTS][notes_MAX];

void MML(int n);

void setup () {
  double x;
 
  /* preparing semitone table */
  for (int i = 0; i <= 4; i++) {
    x = Hz2usec / (C2 * (1 << i));
    semitones[i * 12] = int(x);
    for (int j = 1; j <= 11; j++) {
      x = x / halfstep;
      semitones[i * 12 + j] = int(x);
    }
  }

  for (int n = 0; n < INSTS; n++) {
    pinMode (motor[n], OUTPUT);
    digitalWrite(motor[n], LOW);
    MML(n);
    tick[n] = 0;
    duration[n] = 0;
    laptime[n] = millis();
    nextime[n] = micros();
    period[n]= 0;
  }
}

void initialize()
{
  for (int i = 0; i < INSTS; i++) {
    tick[i] = 0;
    duration[i] = 0;
    laptime[i] = millis();
    nextime[i] = micros();
    period[i]= 0;
  }
}
void loop ()
{
  int i;

  for (i = 0; i < INSTS; i++) {
    /* end of music */
    if (tick[i] > dacapo[i]) {
      digitalWrite(motor[i],0);
      initialize();
      continue;
    }

    /* next note */
    if ((millis() - laptime[i]) >= duration[i]) {
      laptime[i] = millis();
          
      /* read next note */
      if (notes[i][tick[i]] != PAUSE) {
        period[i] = semitones[ abs(notes[i][tick[i]])];
        digitalWrite (motor[i], HIGH);  
      } else {
        period[i] = 0;
        digitalWrite (motor[i], LOW);
      }
      tick[i]++;

      duration[i] = notes[i][tick[i]];
      tick[i]++;

      state[0] = 0;
      nextime[i] = micros();
    }

    /* sound generator */
    if (period[i]) {
      if (micros() > nextime[i]) {
        if (state[i]) {
          nextime[i] = micros() + period[i]*19;
        } else {
          nextime[i] = micros() + period[i];
        }
        state[i] = !state[i];
        digitalWrite (motor[i], state[i]);
      }
    }
  }

}

/*
   an easy-made MML parser
*/
void MML(int n) {
  int octave = 4;
  int length = 8;
  int l;
  long tempo = 120;
  long T120 = 512; /* standard duration */
  boolean tie = false; /* tie mode */
  int i, j, i_s;  /* token */
  char c, m;

  i = 0;
  j = 0;

  while (score(n, i) != 0) {
    c = score(n, i++);    /* read command */
    i_s = i;
    m = score(n, i_s++);  /* read modifier */

    /* to upper case */
    if ((c >= 'a') && (c <= 'z')) {
      c = c - 0x20;
    }

    /* A-G or R : notes and rests */
    if ( ((c >= 'A') && (c <= 'G')) || (c == 'R') ) {
      if (c == 'R') {
        notes[n][j] = PAUSE;
      } else {
        notes[n][j] = diatonic[c - 'A'] + 12 * (octave - 2);
      }

      if (m == '+') {
        notes[n][j] = notes[n][j] + 1;
        m = score(n, i_s++);
      } else if (m == '-') {
        notes[n][j] = notes[n][j] - 1;
        m = score(n, i_s++);
      }
      j++;

      /* read digits */
      if ((m >= '0') && (m <= '9')) {
        l = (m - '0');

        m = score(n, i_s++);   /* read next modifier */
        if ((m >= '0') && (m <= '9')) {
          l = l * 10 + (m - '0');
          m = score(n, i_s++);
        }
      } else {
        l = length;
      }

      notes[n][j] = int(T120 * 120 / tempo * 4 / l);
      if (m == '.') {
        notes[n][j] = notes[n][j] * 3 / 2;
      }

      if (tie) {
        j = j - 2;
        notes[n][j] = notes[n][j] + notes[n][j + 2];
        tie = false;
      }

      j++;

      /* &: tie */
    } else if (c == '&') {
      tie = true;

      /* L: length */
    } else if (c == 'L') {
      if ((m >= '0') && (m <= '9')) {
        length = (m - '0');
        m = score(n, i_s++);   /* read next modifier */
        if ((m >= '0') && (m <= '9')) {
          length = length * 10 + (m - '0');
        }
      }

      /* O: octave changes */
    } else if (c == 'O') {
      if ((m >= '2') && (m <= '6')) {
        octave = (m - '0');
      }

    } else if (c == '<') {
      octave = min(octave + 1, 6);
    } else if (c == '>') {
      octave = max(octave - 1, 2);

      /* T: tempo changes */
    } else if (c == 'T') {
      if ((m >= '0') && (m <= '9')) {
        tempo = (m - '0');
        m = score(n, i_s++);   /* read next modifier */
        if ((m >= '0') && (m <= '9')) {
          tempo = tempo * 10 + (m - '0');
          m = score(n, i_s++);   /* read next modifier */
          if ((m >= '0') && (m <= '9')) {
            tempo = tempo * 10 + (m - '0');
          }
        }
      }
    }
  }

  dacapo[n] = j;
}