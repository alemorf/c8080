VAR X,Y,MAX,X1,Y1,K,Z,D,SH,V:INTEGER;
    MARK:BYTE;
    L,T,O,OT:CHAR;
    ST:STRING[20];
    O1,O2,O3,O4:STRING[5];
LABEL LAB1;

PROCEDURE PRSC;
VAR SC:ARRAY[1..24,1..80] OF CHAR ABSOLUTE $F800;
   X,Y:BYTE;
BEGIN
   FOR Y:=1 TO 24 DO BEGIN
      FOR X:=1 TO 80 DO WRITE(LST,SC[Y,X]);
      WRITELN(LST);
   END;
END;

PROCEDURE XOD(O:CHAR);
BEGIN
   CASE O OF
      '+':BEGIN
             X:=RANDOM(MAX);
             Y:=RANDOM(MAX-X);
             K:=X+Y
          END;
      '-':BEGIN
             X:=RANDOM(MAX);
             Y:=RANDOM(X);
             K:=X-Y
          END;
      '*':BEGIN
             X:=RANDOM(TRUNC(SQRT(MAX))-1)+1;
             Y:=RANDOM(TRUNC(MAX/X));
             K:=X*Y
          END;
      ':':BEGIN
             Y:=RANDOM(TRUNC(SQRT(MAX))-1)+1;
             K:=RANDOM(TRUNC(MAX/Y));
             X:=K*Y
          END;
   END;
   STR(X,O1);STR(Y,O2);
END;

PROCEDURE DOP(O:CHAR;D:INTEGER);
VAR X,Y:INTEGER;
BEGIN
   IF O='+' THEN BEGIN
      X:=RANDOM(D);
      Y:=D-X
   END ELSE BEGIN
      X:=RANDOM(MAX-D)+D;
      Y:=X-D
   END;
   STR(X,O3);STR(Y,O4);
END;

PROCEDURE WWOD;
BEGIN
   CLRSCR;
   SH:=0;V:=0;
   WRITELN('╔════════════════════════════════════════════════════════════════════════════╗');
   WRITELN('║                   ┌───────────────────────────────────┐                    ║');
   WRITELN('║                   │  Решетников И.С. и Роботрон 1715  │                    ║');
   WRITELN('║                   │           представляют            │                    ║');
   WRITELN('║                   └───────────────────────────────────┘                    ║');
   WRITELN('║  ┌──────────────────────────────────────────────────────────────────────┐  ║');
   WRITELN('║  │          Учебная программа для проверки знания математики            │  ║');
   WRITELN('║  │                     учащихся начальной школы.                        │  ║');
   WRITELN('║  │                                                                      │  ║');
   WRITELN('║  │    Сейчас компьютер проверит твои знания по математике. В зависимос- │  ║');
   WRITELN('║  │ ти от нажатой тобой клавиши (1..5) тебе будет предложено 12 примеров │  ║');
   WRITELN('║  │ соответственно на проверку знаний таблицы умножения, на сложение-вы- │  ║');
   WRITELN('║  │ читание, на умножение, на деление или на выполнение сложных действий │  ║');
   WRITELN('║  │ После того,  как появится очередной пример,  подумай и набери ответ, │  ║');
   WRITELN('║  │ используя клавиши с цифрами.  Чтобы исправить последний неверно вве- │  ║');
   WRITELN('║  │ денный символ, используй клавишу <--, а чтоб повторить ввод целиком- │  ║');
   WRITELN('║  │ клавишу !<--. Заканчивай ввод клавишей <ЕТ> или пробелом. После того │  ║');
   WRITELN('║  │ как ты решишь  все  12 примеров,  машина выставит тебе оценку. Время │  ║');
   WRITELN('║  │ при решении не ограничено, но за каждую просроченную минуту добавля- │  ║');
   WRITELN('║  │ ется минута штрафа. За 5 минут штрафа оценка снижается на балл.      │  ║');
   WRITELN('║  │    Для выхода из программы нажми клавишу <ESC>.                      │  ║');
   WRITELN('║  │                                                 Желаю успеха !!!     │  ║');
   WRITELN('║  └──────────────────────────────────────────────────────────────────────┘  ║');
   WRITE('╚════════════════════════════════════════════════════════════════════════════╝');
   REPEAT
      O:=CHR(BIOS(2));
      CASE O OF
         '1'..'5':BEGIN
            CLRSCR;
            WRITELN('╔════════════════════════════════════════════════════════════════════════════╗');
            WRITELN('║                   ┌───────────────────────────────────┐                    ║');
            WRITELN('║                   │  Решетников И.С. и Роботрон 1715  │                    ║');
            WRITELN('║       Всего       │          представляют.            │       Верно        ║');
            WRITELN('║                   └───────────────────────────────────┘                    ║');
            WRITELN('║                          ┌─────────────────────┐                           ║');
            WRITELN('║                   Время  │                     │  Штраф                    ║');
            WRITELN('║      примеров            └─────────────────────┘             решено        ║');
            WRITELN('║                   ┌───────────────────────────────────┐                    ║');
            WRITELN('║                   │             Примеры на            │                    ║');
            WRITELN('║                   │                                   │                    ║');
            WRITELN('║                   └───────────────────────────────────┘                    ║');
            WRITELN('║  ┌──────────────────────────────────────────────────────────────────────┐  ║');
            FOR Z:=1 TO 9 DO WRITELN('║  │','│  ║':74);
            WRITELN('║  └──────────────────────────────────────────────────────────────────────┘  ║');
            WRITE('╚════════════════════════════════════════════════════════════════════════════╝');
            GOTOXY(28,11);
            CASE O OF
               '1':WRITE('знание таблицы умножения');
               '2':WRITE('        сложение');
               '3':WRITE('        умножение');
               '4':WRITE('         деление');
               '5':WRITE('   смешанные действия');
            END;
         END;
         'P','p',^P:PRSC;
         #27:BEGIN CLRSCR;HALT;END;
      END;
   UNTIL O IN ['1'..'5'];
END;

PROCEDURE PLAY;
BEGIN
   IF RANDOM<0.5 THEN T:='+' ELSE T:='-';
   XOD(T);
   D:=RANDOM(7)+1;
   CASE D OF
      1,8:ST:=O1+T+O2;
      7,3,4:BEGIN
             IF RANDOM<0.5 THEN L:='+' ELSE L:='-';
             DOP(L,X);
             IF D=3 THEN ST:=O3+L+O4+T+O2 ELSE ST:='('+O3+L+O4+')'+T+O2;
          END;
      2,6,5:BEGIN
             IF RANDOM<0.5 THEN L:='+' ELSE L:='-';
             DOP(L,Y);
             ST:=O1+T+'('+O3+L+O4+')';
          END
   END
END;

PROCEDURE IGRA;
VAR J1,I,J,P,M,E:BYTE;
    R:INTEGER;
    T:REAL;
    OT1,OT2:STRING[5];
BEGIN
   R:=0;OT1:='';T:=0;M:=0;E:=0;
   GOTOXY(11,6);WRITE(Z:2);
   I:=(Z MOD 4);
   IF I=1 THEN I:=15;
   IF I=2 THEN I:=17;
   IF I=3 THEN I:=19;
   IF I=0 THEN I:=21;
   J:=((Z-1) DIV 4)*22+9;
   GOTOXY(J,I);WRITE(ST);
   J:=J+LENGTH(ST);J1:=J;
   REPEAT
      REPEAT
         T:=T+0.003;
         IF FRAC(T)<0.003 THEN BEGIN
            E:=E+1;
            IF E=60 THEN BEGIN
               E:=0;M:=M+1;SH:=SH+1
            END;
            GOTOXY(30,7);WRITE(M:3,':',E:2,'    <>    ',SH);
            GOTOXY(J1,I);
         END;
      UNTIL (KEYPRESSED OR (M>10));
      IF M>20 THEN EXIT;
      READ (KBD,OT);
      CASE PORT[12] OF
        136:OT:=^H;
        135:OT:=^X;
      END;
      CASE OT OF
        'P',^P:PRSC;
        ^H :BEGIN
           R:=R-1;
           IF R<0 THEN R:=0 ELSE BEGIN
             DELETE(OT1,R+1,3);
             J1:=J+R;
             GOTOXY(J1,I);WRITE(' ');
             GOTOXY(J1,I)
           END
        END;
        ^X :BEGIN
           R:=0;J1:=J;OT1:='';
           GOTOXY(J1,I);WRITE('      ');
           GOTOXY(J1,I);
        END;
        '0'..'9':BEGIN
           IF R<6 THEN BEGIN
             OT1:=OT1+OT;
             GOTOXY(J1,I);WRITE(OT);
             R:=R+1;J1:=J+R;
           END
         END;
         #27:EXIT;
      END;
   UNTIL ((OT=' ') OR (M>10) OR (OT=^M));
   STR(K,OT2);
   IF OT1='' THEN  ST:='Ответ : '+OT2+'.' ELSE
   IF OT1=OT2 THEN ST:='Молодец, верно.' ELSE ST:='Неправильно !!!';
   IF OT1=OT2 THEN BEGIN
      V:=V+1;
      GOTOXY(66,6);WRITE(V:2);
   END ELSE BEGIN GOTOXY(((Z-1) DIV 4)*22+8,I);WRITE(#7'*');END;
   GOTOXY(32,23);WRITE(' ',ST,' ');
   DELAY(2000);
   GOTOXY(32,23);WRITE('─────────────────────');
END;

BEGIN
   MAX:=100;
   VAL(PARAMSTR(1),MAX,Z);
   IF (Z<>0) OR (MAX<100) THEN MAX:=100;
   REPEAT
      WWOD;
      FOR Z:=1 TO 12 DO BEGIN
         CASE O OF
            '1':BEGIN
                   X:=RANDOM(10);Y:=RANDOM(10);
                   K:=X*Y;
                   STR(X,O1);STR(Y,O2);
                   ST:=O1+'*'+O2
                END;
            '2':PLAY;
            '3','4':BEGIN
                   IF O='3' THEN L:='*' ELSE L:=':';
                   XOD(L);
                   ST:=O1+L+O2
                END;
            '5':BEGIN
                   CASE RANDOM(5) OF
                      0,5:T:='+';
                      3,2:T:=':';
                      1,4:T:='*';
                   END;
                   IF T='+' THEN PLAY ELSE BEGIN
                      XOD(T);
                      IF RANDOM<0.5 THEN L:='+' ELSE L:='-';
                      CASE RANDOM(7) OF
                         0,7:BEGIN
                              DOP(L,X);
                              ST:='('+O3+L+O4+')'+T+O2
                             END;
                         1,6:BEGIN
                              DOP(L,Y);
                              ST:=O1+T+'('+O3+L+O4+')'
                             END;
                         2,5:BEGIN
                              IF RANDOM <0.5 THEN BEGIN
                                 X1:=RANDOM(MAX-K)+K;
                                 K:=X1-K;STR(X1,O3);
                                 ST:=O3+'-'+O1+T+O2
                              END ELSE
                              BEGIN
                                 X1:=RANDOM(MAX-K);
                                 K:=K+X1;STR(X1,O3);
                                 ST:=O1+T+O2+'+'+O3;
                              END
                             END;
                         3,4:BEGIN
                              IF RANDOM<0.5 THEN BEGIN
                                 X1:=RANDOM(K);
                                 K:=K-X1;STR(X1,O3);
                                 ST:=O1+T+O2+'-'+O3;
                              END ELSE BEGIN
                                 X1:=RANDOM(MAX-K);
                                 K:=K+X1;STR(X1,O3);
                                 ST:=O3+'+'+O1+T+O2;
                              END
                             END;
                   END;
               END;
            END                          
         END;
         ST:=ST+'=';
         IGRA;IF OT=#27 THEN GOTO LAB1;
      END;
      CLRSCR;
      MARK:=V DIV 3+1-SH DIV 5;
      IF MARK<2 THEN MARK:=2;
      GOTOXY(30,12);WRITELN('Твоя оценка  : ',MARK:1);
      GOTOXY(25,14);
      CASE MARK OF
         5:WRITE('        Отлично !!!');
         4:WRITE('       Хорошо, но...');
         3:WRITE('Удовлетворительно, поработай еще.');
         2:WRITE('       Очень плохо...');
      END;
      GOTOXY(2,24);WRITE('Нажми любую клавишу...');
      BIOS(2);
LAB1:UNTIL FALSE;
END.
END.
                 