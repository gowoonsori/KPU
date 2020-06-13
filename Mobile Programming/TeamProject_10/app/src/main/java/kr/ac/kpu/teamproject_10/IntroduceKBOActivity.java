package kr.ac.kpu.teamproject_10;

import android.content.Intent;
import android.graphics.Color;
import android.graphics.PorterDuff;
import android.graphics.drawable.Drawable;
import android.net.Uri;
import android.os.Bundle;
import android.view.MenuItem;
import android.view.View;
import android.widget.AdapterView;
import android.widget.ArrayAdapter;
import android.widget.Button;
import android.widget.ImageButton;
import android.widget.Spinner;
import android.widget.TextView;

import androidx.appcompat.app.AppCompatActivity;
public class IntroduceKBOActivity extends AppCompatActivity {

    ImageButton news_bt,youtube_bt,homepage_bt;
    TextView inf_tv;
    ArrayAdapter<CharSequence> adspin1,adspin2;  //두개의 스피너 item붙이기위한 adapter

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_introduce_kbo);

        // 타이틀 바 수정구현
        setTitle("리그 소개");
        getSupportActionBar().setDisplayHomeAsUpEnabled(true);
        getSupportActionBar().setDisplayShowHomeEnabled(true);
        getSupportActionBar().setIcon(R.drawable.kbo2020);

        /*버튼 event listener*/
        news_bt=(ImageButton)findViewById(R.id.newsBt);
        youtube_bt=(ImageButton)findViewById(R.id.youtubeBt);
        homepage_bt=(ImageButton)findViewById(R.id.homepageBt);

        /*Intent Chrome news*/
        news_bt.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                Uri uri = Uri.parse("https://sports.news.naver.com/kbaseball/index.nhn");
                Intent intent =new Intent(Intent.ACTION_VIEW,uri);
                startActivity(intent);
            }
        });
        /*Intent youtube어플*/
        youtube_bt.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                Uri uri = Uri.parse("https://www.youtube.com/results?search_query=kbo");
                Intent intent =new Intent(Intent.ACTION_VIEW,uri);
                startActivity(intent);
            }
        });
        /*Intent kbo홈페이지*/
        homepage_bt.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                Uri uri = Uri.parse("https://www.koreabaseball.com/Default.aspx");
                Intent intent =new Intent(Intent.ACTION_VIEW,uri);
                startActivity(intent);
            }
        });

        /*2개의 스피너 event */
        final Spinner inf_sp=(Spinner)findViewById(R.id.inf_spinner);
        final Spinner subInf_sp=(Spinner)findViewById(R.id.subInf_spinner);
        inf_tv=(TextView)findViewById(R.id.inf_tv);

        //첫번째 스피너에 inf_spinner_array 붙이기
        adspin1=ArrayAdapter.createFromResource(this,R.array.inf_spinner_array,R.layout.support_simple_spinner_dropdown_item);
        inf_sp.setAdapter(adspin1);
        //스피너 이벤트 리스너
        inf_sp.setOnItemSelectedListener(new AdapterView.OnItemSelectedListener() {
            @Override
            public void onItemSelected(AdapterView<?> adapterView, View view, int item, long l) {
                /*KBO 소개*/
                if(item==0){
                    adspin2=ArrayAdapter.createFromResource(IntroduceKBOActivity.this,R.array.subinf_spinner_array1,R.layout.support_simple_spinner_dropdown_item);
                    subInf_sp.setAdapter(adspin2);
                    subInf_sp.setOnItemSelectedListener(new AdapterView.OnItemSelectedListener() {
                        @Override
                        //각 질문에 맞는 대답 string.xml에서 불러와 TextView에 set
                        public void onItemSelected(AdapterView<?> adapterView, View view, int i, long l) {
                            if(i==0)inf_tv.setText(R.string.KBOLeague);
                            else if(i==1)inf_tv.setText(R.string.currentClub);
                            else if(i==2)inf_tv.setText(R.string.stadium);
                        }
                        @Override
                        public void onNothingSelected(AdapterView<?> adapterView) { }});
                }
                //경기 운영 관련 질문
                if(item==1){
                    adspin2=ArrayAdapter.createFromResource(IntroduceKBOActivity.this,R.array.subinf_spinner_array2,R.layout.support_simple_spinner_dropdown_item);
                    subInf_sp.setAdapter(adspin2);

                    subInf_sp.setOnItemSelectedListener(new AdapterView.OnItemSelectedListener() {
                        @Override
                        //각 질문에 맞는 대답 string.xml에서 불러와 TextView에 set
                        public void onItemSelected(AdapterView<?> adapterView, View view, int i, long l) {
                            if(i==0)inf_tv.setText(R.string.MatchOperationSystem);
                            else if(i==1)inf_tv.setText(R.string.cancelPolicy_rain);
                            else if(i==2)inf_tv.setText(R.string.cancelPolicy_yellowDust);
                            else if(i==3)inf_tv.setText(R.string.matchStartTime);
                            else if(i==4)inf_tv.setText(R.string.Overtime);
                        }
                        @Override
                        public void onNothingSelected(AdapterView<?> adapterView) { }});
                }
                //식음료 관련 질문
                if(item==2){
                    adspin2=ArrayAdapter.createFromResource(IntroduceKBOActivity.this,R.array.subinf_spinner_array3,R.layout.support_simple_spinner_dropdown_item);
                    subInf_sp.setAdapter(adspin2);
                    subInf_sp.setOnItemSelectedListener(new AdapterView.OnItemSelectedListener() {
                        @Override
                        //각 질문에 맞는 대답 string.xml에서 불러와 TextView에 set
                        public void onItemSelected(AdapterView<?> adapterView, View view, int i, long l) {
                            if(i==0)inf_tv.setText(R.string.Alcohol);
                            else if(i==1)inf_tv.setText(R.string.Food);
                            else if(i==2)inf_tv.setText(R.string.smoking);
                        }
                        @Override
                        public void onNothingSelected(AdapterView<?> adapterView) { }});
                }
                //티켓 관련 질문
                if(item==3){
                    adspin2=ArrayAdapter.createFromResource(IntroduceKBOActivity.this,R.array.subinf_spinner_array4,R.layout.support_simple_spinner_dropdown_item);
                    subInf_sp.setAdapter(adspin2);
                    subInf_sp.setOnItemSelectedListener(new AdapterView.OnItemSelectedListener() {
                        @Override
                        public void onItemSelected(AdapterView<?> adapterView, View view, int i, long l) {
                            if (i == 0) inf_tv.setText(R.string.ticket);
                        }
                        @Override
                        public void onNothingSelected(AdapterView<?> adapterView) { }});
                }
            }
            @Override
            public void onNothingSelected(AdapterView<?> adapterView) { }
        });
    }

    /*뒤로가기 버튼 이벤트*/
    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        switch (item.getItemId()){
            case android.R.id.home:{ //toolbar의 back키 눌렀을 때 동작
                finish();
                return true;
            }
        }
        return super.onOptionsItemSelected(item);
    }
}
/*Icon 출처
https://br.pinterest.com/pin/605945324846287987/
https://www.vectorstock.com/royalty-free-vector/newspaper-line-web-newsletter-or-news-app-icon-vector-23828511
https://thenounproject.com/term/homepage/
 */