package kr.ac.kpu.teamproject_10;
import android.content.Intent;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.net.Uri;
import android.os.Bundle;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.widget.Button;
import android.widget.ImageView;
import android.widget.LinearLayout;
import android.widget.RatingBar;
import android.widget.TextView;
import android.widget.ViewFlipper;
import androidx.appcompat.app.AppCompatActivity;
public class SelectedTeamActivity extends AppCompatActivity {
    ImageView ivImage, rightBtn, leftBtn;
    TextView textView, text_01, text_02, text_03, text_04, text_05, rating_text;
    LinearLayout linear01, linear02, linear03, linear04, linear05;
    Button cancel_button, ok_button;
    String []info ={ "", "", "", "", "", "" }; // 연고지, 홈구장, 구단홈페이지, 순위, 창단연도 , 위/경도
    ViewFlipper view;
    RatingBar ratingBar;
    float num;

    // 옵션 메뉴 생성 및 선택 리스너 구현
    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        getMenuInflater().inflate(R.menu.previous, menu);
        return true;
    }
    // 옵션 메뉴 클릭시 선택 리스너 구현
    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        switch (item.getItemId()){
            case android.R.id.home: // toolbar의 back키 눌렀을 때 동작
                finish();
                return true;
            case R.id.returnhome: // 홈 바로가기 메뉴 눌렀을 때 동작
                startActivity(new Intent(this, MainActivity.class));
                return true;
        }
        return super.onOptionsItemSelected(item);
    }
    String []homepage= { // 구단 홈페이지
            "https://www.doosanbears.com/" ,
            "https://www.heroesbaseball.co.kr/index.do",
            "https://www.skwyverns.com/Wyverns/main.asp",
            "https://www.lgtwins.com/service/html.ncd?view=/pc_twins/twins_main/twins_main",
            "https://ncdinos.com/main/index",
            "https://www.ktwiz.co.kr/" ,
            "https://tigers.co.kr/" ,
            "http://www.samsunglions.com/",
            "http://www.hanwhaeagles.co.kr/html/main/main.asp",
            "http://www.giantsclub.com/html/"
    };
    String [] address ={ // 연고지
            "서울특별시","서울특별시" ,"인천광역시","서울특별시", "경상남도 창원시",
            "경기도 수원시", "광주광역시", "대구광역시", "대전광역시", "부산광역시"
    };
    String [] home ={ // 홈구장
            "잠실 야구장" , "고척 스카이돔", "인천 sk행복드림구장", "잠실야구장",
            "창원NC파크", "수원KT위즈파크", "광주 기아 챔피언스필드", "대구 삼성 라이온즈 파크",
            "한화생명 이글스 파크", "사직야구장"
    };
    String [] year={ // 창단 년도
            "1982" , "2008", "2000", "1990", "2011", "2013", "2001", "1982", "1986", "1982"
    };
    String [] googleMap = { // 홈구장 좌표
            "http://maps.google.com/maps?q="+"잠실야구장", //잠실
            "http://maps.google.com/maps?q="+"고척스카이돔", //고척 스카이돔
            "http://maps.google.com/maps?q="+"인천 sk행복드림구장", //인천
            "http://maps.google.com/maps?q="+"잠실야구장", //잠실
            "http://maps.google.com/maps?q="+"창원NC파크", //nc 35.2225225,128.5800144
            "http://maps.google.com/maps?q="+"수원KT위즈파크", //수원kt위즈파크37.3002955,127.0073141
            "http://maps.google.com/maps?q="+"광주 기아 챔피언스필드", //광주 기아 챔피언스 필드35.1681077,126.8868112
            "http://maps.google.com/maps?q="+"대구 삼성 라이온즈 파크", //"대구 삼성 라이온즈 파크",35.8407803,128.6790603
            "http://maps.google.com/maps?q="+"한화생명 이글스 파크", //한화 생명 이글스 파크 //36.3170683,127.426953
            "http://maps.google.com/maps?q="+"사직야구장", //사직 야구장35.1940316,129.0593296
    };
    int [][] image = { // 뷰플리퍼에 출력되는 이미지들
            {R.drawable.photo_bears1, R.drawable.photo_bears2, R.drawable.photo_bears3, R.drawable.photo_bears4, R.drawable.photo_bears5},
            {R.drawable.photo_heroes1, R.drawable.photo_heroes2, R.drawable.photo_heroes3, R.drawable.photo_heroes4, R.drawable.photo_heroes5},
            {R.drawable.photo_wyverns1, R.drawable.photo_wyverns2, R.drawable.photo_wyverns3, R.drawable.photo_wyverns4, R.drawable.photo_wyverns5},
            {R.drawable.photo_twins1,R.drawable.photo_twins2,R.drawable.photo_twins3,R.drawable.photo_twins4,R.drawable.photo_twins5},
            {R.drawable.photo_dinos1,R.drawable.photo_dinos2,R.drawable.photo_dinos3,R.drawable.photo_dinos4,R.drawable.photo_dinos5},
            {R.drawable.photo_wiz1,R.drawable.photo_wiz2,R.drawable.photo_wiz3,R.drawable.photo_wiz4,R.drawable.photo_wiz5},
            {R.drawable.photo_tigers1,R.drawable.photo_tigers2,R.drawable.photo_tigers3,R.drawable.photo_tigers4,R.drawable.photo_tigers5},
            {R.drawable.photo_lions1,R.drawable.photo_lions2,R.drawable.photo_lions3,R.drawable.photo_lions4,R.drawable.photo_lions5},
            {R.drawable.photo_eagles1,R.drawable.photo_eagles2,R.drawable.photo_eagles3,R.drawable.photo_eagles4,R.drawable.photo_eagles5},
            {R.drawable.photo_giants1,R.drawable.photo_giants2,R.drawable.photo_giants3,R.drawable.photo_giants4,R.drawable.photo_giants5}
    };
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_selected_team);

        // 타이틀 바 설정(텍스트, 아이콘), 툴바 설정 | 색상은 styles.xml에서 작성
        getSupportActionBar().setDisplayShowHomeEnabled(true);
        getSupportActionBar().setIcon(R.drawable.person);
        getSupportActionBar().setDisplayHomeAsUpEnabled(true); // 툴바

        ivImage = findViewById(R.id.imageView);
        textView = findViewById(R.id.textView);
        text_01 = findViewById(R.id.text_01);
        text_02 = findViewById(R.id.text_02);
        text_03 = findViewById(R.id.text_03);
        text_04 = findViewById(R.id.text_04);
        text_05 = findViewById(R.id.text_05);
        view=findViewById(R.id.viewFlip);
        rightBtn = findViewById(R.id.right);
        leftBtn = findViewById(R.id.left);
        linear01= findViewById(R.id.linear1);
        linear02= findViewById(R.id.linear2);
        linear03= findViewById(R.id.linear3);
        linear04= findViewById(R.id.linear4);
        linear05= findViewById(R.id.linear5);
        ratingBar = findViewById(R.id.rating);
        rating_text = findViewById(R.id.rating_text);
        cancel_button = findViewById(R.id.btn_cancel);
        ok_button = findViewById(R.id.btn_ok);

        //IntrodeceActivity에서 가져온 이미지와 타이틀을 객체에 저장
        Intent intent= getIntent();
        String str = intent.getStringExtra("title");
        byte[] byteArray = intent.getByteArrayExtra("image");
        float starNum= intent.getFloatExtra("number",num);
        Bitmap image = BitmapFactory.decodeByteArray(byteArray, 0, byteArray.length);

        textView.setText(str);
        ivImage.setImageBitmap(image);
        view.startFlipping();
        ratingBar.setRating(starNum);
        rating_text.setText(""+starNum);


        setRatinBar();      //레이팅 바의 점수를 위한 함수
        check_str(str);     // 해당 화면에 보여질 정보들 설정을 위한 함수
        click_add();        // 구글 맵 이동
        click_homepage();   // 홈페이지 이동
        click();            //플립뷰 이미지 양옆 이동
        click_button(str);  //확인 취소버튼
    }
    //확인, 취소버튼. 확인버튼->결과값으로 Rating값으로 저장된 실수를 넘겨줌
    private void click_button(final String str) {
        ok_button.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                Intent intent = new Intent(SelectedTeamActivity.this, IntroduceTeamActivity.class);
                intent.putExtra(str, num);
                setResult(RESULT_OK, intent);
                finish();
            }
        });
        cancel_button.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                finish();
            }
        });
    }
    //레이팅바 점수를 표시, 저장하기 위한 메소드
    private void setRatinBar() {
        ratingBar.setOnRatingBarChangeListener(new RatingBar.OnRatingBarChangeListener() {
            @Override
            public void onRatingChanged(RatingBar ratingBar, float rating, boolean fromUser) {
                rating_text.setText("\t"+rating);
                num=rating;
            }
        });
    }
    // 플립뷰 이미지 이동을 위한 메소드
    private void click() {
        rightBtn.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                view.showNext();
            }
        });
        leftBtn.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                view.showPrevious();
            }
        });
    }
    //암시적 인텐트로 홈구장 지도로 이동하는 메소드
    private void click_add() {
        text_02.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                Uri uri = Uri.parse(info[5]);
                Intent intent = new Intent(Intent.ACTION_VIEW, uri);
                startActivity(intent);
            }
        });
    }
    //암시적 인텐트로 해당 홈페이지로 이동하는 메소드
    private void click_homepage() {
        text_03.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                Uri uri = Uri.parse(info[2]);
                Intent intent = new Intent(Intent.ACTION_VIEW, uri);
                startActivity(intent);
            }
        });
    }
    //타이틀이름을 기준으로 화면에 표시될 정보들을 저장및 출력하고, 플립뷰의 이미지를 변경하는 메소드
    private void check_str(String str) {
        switch (str){
            case "두산 베어스" :
                setTitle(" 두산 베어스");
                info[0] = address[0];
                info[1] = home[0];
                info[2] = homepage[0];
                info[3] = "1";
                info[4] = year[0];
                info[5] = googleMap[0];
                linear01.setBackgroundResource(image[0][0]);
                linear02.setBackgroundResource(image[0][1]);
                linear03.setBackgroundResource(image[0][2]);
                linear04.setBackgroundResource(image[0][3]);
                linear05.setBackgroundResource(image[0][4]);
                break;
            case "키움 히어로즈" :
                setTitle(" 키움 히어로즈");
                info[0] = address[1];
                info[1] = home[1];
                info[2] = homepage[1];
                info[3] = "2";
                info[4] = year[1];
                info[5] = googleMap[1];
                linear01.setBackgroundResource(image[1][0]);
                linear02.setBackgroundResource(image[1][1]);
                linear03.setBackgroundResource(image[1][2]);
                linear04.setBackgroundResource(image[1][3]);
                linear05.setBackgroundResource(image[1][4]);
                break;
            case "SK 와이번스" :
                setTitle(" SK 와이번스");
                info[0] = address[2];
                info[1] = home[2];
                info[2] = homepage[2];
                info[3] = "3";
                info[4] = year[2];
                info[5] = googleMap[2];
                linear01.setBackgroundResource(image[2][0]);
                linear02.setBackgroundResource(image[2][1]);
                linear03.setBackgroundResource(image[2][2]);
                linear04.setBackgroundResource(image[2][3]);
                linear05.setBackgroundResource(image[2][4]);
                break;
            case "LG 트윈스" :
                setTitle(" LG 트윈스");
                info[0] = address[3];
                info[1] = home[3];
                info[2] = homepage[3];
                info[3] = "4";
                info[4] = year[3];
                info[5] = googleMap[3];
                linear01.setBackgroundResource(image[3][0]);
                linear02.setBackgroundResource(image[3][1]);
                linear03.setBackgroundResource(image[3][2]);
                linear04.setBackgroundResource(image[3][3]);
                linear05.setBackgroundResource(image[3][4]);
                break;
            case "NC 다이노스" :
                setTitle(" NC 다이노스");
                info[0] = address[4];
                info[1] = home[4];
                info[2] = homepage[4];
                info[3] = "5";
                info[4] = year[4];
                info[5] = googleMap[4];
                linear01.setBackgroundResource(image[4][0]);
                linear02.setBackgroundResource(image[4][1]);
                linear03.setBackgroundResource(image[4][2]);
                linear04.setBackgroundResource(image[4][3]);
                linear05.setBackgroundResource(image[4][4]);
                break;
            case "KT 위즈" :
                setTitle(" KT 위즈");
                info[0] = address[5];
                info[1] = home[5];
                info[2] = homepage[5];
                info[3] = "6";
                info[4] = year[5];
                info[5] = googleMap[5];
                linear01.setBackgroundResource(image[5][0]);
                linear02.setBackgroundResource(image[5][1]);
                linear03.setBackgroundResource(image[5][2]);
                linear04.setBackgroundResource(image[5][3]);
                linear05.setBackgroundResource(image[5][4]);
                break;
            case "KIA 타이거즈" :
                setTitle(" KIA 타이거즈");
                info[0] = address[6];
                info[1] = home[6];
                info[2] = homepage[6];
                info[3] = "7";
                info[4] = year[6];
                info[5] = googleMap[6];
                linear01.setBackgroundResource(image[6][0]);
                linear02.setBackgroundResource(image[6][1]);
                linear03.setBackgroundResource(image[6][2]);
                linear04.setBackgroundResource(image[6][3]);
                linear05.setBackgroundResource(image[6][4]);
                break;
            case "삼성 라이온즈" :
                setTitle(" 삼성 라이온즈");
                info[0] = address[7];
                info[1] = home[7];
                info[2] = homepage[7];
                info[3] = "8";
                info[4] = year[7];
                info[5] = googleMap[7];
                linear01.setBackgroundResource(image[7][0]);
                linear02.setBackgroundResource(image[7][1]);
                linear03.setBackgroundResource(image[7][2]);
                linear04.setBackgroundResource(image[7][3]);
                linear05.setBackgroundResource(image[7][4]);
                break;
            case "한화 이글스" :
                setTitle(" 한화 이글스");
                info[0] = address[8];
                info[1] = home[8];
                info[2] = homepage[8];
                info[3] = "9";
                info[4] = year[8];
                info[5] = googleMap[8];
                linear01.setBackgroundResource(image[8][0]);
                linear02.setBackgroundResource(image[8][1]);
                linear03.setBackgroundResource(image[8][2]);
                linear04.setBackgroundResource(image[8][3]);
                linear05.setBackgroundResource(image[8][4]);
                break;
            case "롯데 자이언츠" :
                setTitle(" 롯데 자이언츠");
                info[0] = address[9];
                info[1] = home[9];
                info[2] = homepage[9];
                info[3] = "10";
                info[4] = year[9];
                info[5] = googleMap[9];
                linear01.setBackgroundResource(image[9][0]);
                linear02.setBackgroundResource(image[9][1]);
                linear03.setBackgroundResource(image[9][2]);
                linear04.setBackgroundResource(image[9][3]);
                linear05.setBackgroundResource(image[9][4]);
                break;
        }
        // 위에서 설정한 값들을 표시
        text_01.setText(info[0]);
        text_02.setText(info[1]);
        text_03.setText("구단 홈페이지 바로가기");
        text_04.setText(info[3]);
        text_05.setText(info[4]);
    }
}