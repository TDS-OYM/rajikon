/*
 * File:   receiver.c
 * Author: TDS001
 *
 * Created on December 13, 2024, 11:59 PM
 */
#include "PIC18F23K20.h"
#define _XTAL_FREQ 16000000

//MoterDriver入力用
#define Ain2 PORTBbits.RB0
#define Ain1 PORTBbits.RB1
#define STBY PORTBbits.RB2
#define Bin1 PORTBbits.RB3
#define Bin2 PORTBbits.RB4
#define APWM PORTCbits.RC1
#define BPWM PORTCbits.RC2

//変数
char direction;

void main(void) {
    
    //メインクロックの設定
    OSCCONbits.IRCF=0b111;//16MHz
    
    //入出力設定
    //出力モード
    TRISBbits.RB0=0;
    TRISBbits.RB1=0;
    TRISBbits.RB2=0;
    TRISBbits.RB3=0;
    TRISBbits.RB4=0;
    
    //PWM用のCCP1,CCP2を出力に設定
    TRISCbits.RC1=0;
    TRISCbits.RC2=0;
    
    
    //初期設定
    STBY=1;//モータードライバーのSTBYピンをONにしておく
    
    //PWM設定
    //CCP1,CCP2をPWMモードに設定
    CCP1CONbits.CCP1M=0b1100;
    CCP2CONbits.CCP2M=0b1100;
    
    CCPR1L=0b10000000;//CCP1ピンのPWM出力100%
    CCPR2L=0b10000000;//CCP2ピンのPWM出力50%
    
    
    //タイマ2を使用したデューティサイクルの設定
	//タイマ2とCCP1,CCCP2との割合でデューティサイクルの比率を決める
    T2CONbits.TMR2ON=1;//設定のためにタイマ2を起動
    T2CONbits.T2OUTPS0=1;//ポストスケーラー1：1
    T2CONbits.T2CKPS0=0b00;//プリスケーラーが1：1
    PR2=255;
    
    INTCONbits.PEIE=1;//周辺割り込みの許可
    INTCONbits.GIE=1;//全体割り込みの許可
    
    
    T2CONbits.TMR2ON=0;//設定の終了にタイマ2を止める
    
    
    
    //UART設定
    TRISCbits.RC6=0;//TX
    TRISCbits.RC7=1;//RX
    
    SPBRG=8;//16MHzの時、115.2kbps
    
    //UART設定//非同期モードでの設定
    TXSTAbits.SYNC=0;//非同期モード
    TXSTAbits.BRGH=1;//高速ボーレート
    BAUDCONbits.BRG16=0;//8bitボーレートジェネレータ
    
    
    //UART設定//受信の設定
    TXSTAbits.TXEN=1;//送信機の有効化//連続送信許可
    RCSTAbits.CREN=1;//受信機の有効化//連続受信許可
    RCSTAbits.SPEN=1;//シリアルポートの有効化
    
    //割込み設定
    
    PIE1bits.TMR2IE=1;//タイマ2割込み設定
    PIE1bits.RCIE=1;//UART受信割込みの許可
    INTCONbits.PEIE=1;//周辺割り込みの許可
    INTCONbits.GIE=1;//全体割り込みの許可
    
    T2CONbits.TMR2ON=1; //タイマ2スタート
   
    while(1){
  
    }
    
     
    return;
}


//外部割込み関数
//UART受信
void __interrupt() isr(void){
    if(PIR1bits.RCIF==1){//コントローラー側からの情報を受信したら
        direction=RCREG;//受信データを変数directionに入れる
    }
    
    if(direction=='L'){//受信したデータが'L'の時
        /*アナログスティックの左入力50以上=Lの場合は左に曲がる
        左側のモーターの回転速度を落とす*/
        CCPR1L=0b10000000;//CCP1ピンのPWM出力100%
        CCPR2L=0b11111111;//CCP2ピンのPWM出力30%
        Ain1=1, Ain2=0;
        Bin1=1, Bin2=0;
        APWM=1, BPWM=1;
    }
    else if(direction=='R'){//受信したデータが'R'の時
     /*アナログスティックの右入力50以上=Rの場合は右に曲がる
     右側のモーターの回転速度を落とす*/
        CCPR1L=0b11111111;//CCP1ピンのPWM出力100%
        CCPR2L=0b10000000;//CCP2ピンのPWM出力30%
        Ain1=1, Ain2=0;
        Bin1=1, Bin2=0;
        APWM=1, BPWM=1;
    }
    else if(direction=='U'){//受信したデータが'U'の時
        /*アナログスティックの上入力50以上=Uの場合は直進する
        左右のモーターの回転速度は同じ*/
        CCPR1L=0b11111111;//CCP1ピンのPWM出力100%
        CCPR2L=0b11111111;//CCP2ピンのPWM出力100%
        Ain1=1, Ain2=0;
        Bin1=1, Bin2=0;
        APWM=1, BPWM=1;
    }
    else if(direction=='D'){//受信したデータが'D'の時
     /*アナログスティックの下入力50以上=Dの場合はバックする
     左右のモーターの回転速度は同じだが回転方向は逆方向*/
        
        CCPR1L=0b11111111;//CCP1ピンのPWM出力100%
        CCPR2L=0b11111111;//CCP2ピンのPWM出力100% 
        Ain1=0, Ain2=1;
        Bin1=0, Bin2=1;
        APWM=1, BPWM=1;
    }
    else{
    /*アナログスティックの入力がない状態の時、あるいは
    中途半端な入力の時はストップ*/
        Ain1=0, Ain2=0;
        Bin1=0, Bin2=0;
        BPWM=1, APWM=1;
    }
}
          