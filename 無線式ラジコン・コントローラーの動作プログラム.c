#include "PIC18F23K20.h"
#define _XTAL_FREQ 16000000

//MaxChar関数:ジョイスティックを傾けた方向を取り出し、U,D,R,Lで返す
char MaxChar(int L, int R, int U, int D)
{
	char max='L';
    int num=L;
    
	if(num<R){
		max='R';
        num=R;
	}
	
	if(num<U){
		max='U';
        num=U;
	}
	
	if(num<D){
		max='D';
        num=D;
	}
	
	return max;
}




void main(void)
{
	//メインクロックの設定
	OSCCONbits.IRCF=0b111;//16MHz
	
	//AD変換設定
	TRISAbits.RA0=1;
	TRISAbits.RA1=1;
	ANSELbits.ANS0=1;//RA0をアナログ入力ポートにする
	ANSELbits.ANS1=1;//RA1をアナログ入力ポートにする
	
	ADCON2bits.ADCS=0b111;//専用内部オシレータの利用
	ADCON1bits.VCFG=0b00;//電圧リファレンスをVDD、VSSを基準とする
	ADCON2bits.ADFM=1;//結果フォーマットを右詰めにする
	ADCON2bits.ACQT=0b101;//アクイジション時間を12TADに設定
	ADCON0bits.ADON=1;//ADCモジュールをON
	
	//UART設定
	TRISCbits.RC6=1;//RX入力設定
	TRISCbits.RC7=1;//TX入力設定
    
     SPBRG=8;//16MHzの時、115.2kbps
	
	//UART設定での非同期モードでの設定
	TXSTAbits.SYNC=0;//非同期モード
	TXSTAbits.BRGH=1;//高速ボーレート
	BAUDCONbits.BRG16=0;//8bitボーレートジェネレータ
	
	//UART設定での送受信の設定
	TXSTAbits.TXEN=1;//送信機の有効化：連続送信許可
	RCSTAbits.SPEN=1;//シリアルポートの有効化
	
    
	//変数の宣言
	unsigned int LR, UD;//ジョイスティックの左右、上下の入力値を格納
	unsigned int L,R,U,D;//ジョイスティックの入力値を左右上下に0～100に変換したものを格納
    char direction;
	
		
	while(1){
		//左右のデータ
		ADCON0bits.CHS=0b0000;//ADC入力チャンネルをAN0に接続
		ADCON0bits.GO_nDONE=1;//AD変換開始
		
		while(ADCON0bits.GO_nDONE==1){};//AD変換が終わるまで待機
		LR=ADRES;//LRにジョイスティックの左右の入力値を格納する
		
		if(LR>=512){//ジョイスティックを右に傾けたとき(ジョイスティックは未入力だと512になる)
			LR=LR-512;
			R=LR*100/512;//Rに入力値を0～100に変換した数値を格納
			L=0;//Lに0を格納する
		}
		else{
			LR=512-LR;
			L=LR*100/512;//Lに入力値を0～100に変換した数値を格納
			R=0;//Rに0を格納する
		}
		
		ADCON0bits.CHS=0b0001;//ADC入力チャンネルをAN1に接続
		ADCON0bits.GO_nDONE=1;//AD変換開始
		while(ADCON0bits.GO_nDONE==1){};//AD変換が終わるまで待機
		UD=ADRES;//UDにジョイスティックの上下の入力値を格納する
		
		if(UD>=512){//ジョイスティックを右に傾けたとき(ジョイスティックは未入力だと512になる)
			UD=UD-512;
			U=UD*100/512;//Uに入力値を0～100に変換した数値を格納
			D=0;//Dに0を格納する
		}
		else{
			UD=512-UD;
			D=UD*100/512;//Dに入力値を0～100に変換した数値を格納
			U=0;//Uに0を格納する
		}
		
		//ジョイスティックを傾けた方向をL,R,U,Dで取り出す
		direction=MaxChar(L,R,U,D);
        
        if(direction=='L' && L>=50){//ジョイスティックを左側に半分以上傾けた時
             TXREG='L';//'L'を送信
        }
        else if(direction=='R' && R>=50){//ジョイスティックを右側に半分以上傾けた時
             TXREG='R';//'R'を送信
        }
        else if(direction=='U' && U>=50){//ジョイスティックを上側に半分以上傾けた時
             TXREG='U';//'U'を送信
        }
        else if(direction=='D' && D>=50){//ジョイスティックを下側に半分以上傾けた時
             TXREG='D';//'D'を送信
        }
        else{
            TXREG='S';//ジョイスティックを操作していない時、ジョイスティックの入力が各方向に半分未満の時は'S'を送信
        }
       
		while(TXSTAbits.TRMT==0){}//送信終了までポーリング
	}
	
	return;
}

