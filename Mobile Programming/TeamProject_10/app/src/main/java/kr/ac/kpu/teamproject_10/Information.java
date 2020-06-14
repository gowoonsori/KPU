package kr.ac.kpu.teamproject_10;
import android.content.Intent;
import android.net.Uri;
import android.os.Bundle;
import android.view.MenuItem;
import android.view.View;
import android.widget.AdapterView;
import android.widget.ArrayAdapter;
import android.widget.ListView;
import android.widget.TextView;
import androidx.appcompat.app.AppCompatActivity;
public class Information extends AppCompatActivity {

    TextView textView; // KBO 어플 구글플레이 접속을 위한 객체 생성

    @Override // 툴바 뒤로가기 기능
    public boolean onOptionsItemSelected(MenuItem item) {
        switch (item.getItemId()){
            case android.R.id.home:{ //toolbar의 back키 눌렀을 때 동작
                finish(); return true; // 이전 액티비티로 회귀
            }
        } return super.onOptionsItemSelected(item);
    }
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_information);

        // 타이틀 바 설정(텍스트, 아이콘), 툴바 설정 | 색상은 styles.xml에서 작성
        setTitle("애플리케이션 정보");
        getSupportActionBar().setDisplayShowHomeEnabled(true);
        getSupportActionBar().setIcon(R.drawable.kbo_2020_logo);
        getSupportActionBar().setDisplayHomeAsUpEnabled(true); // 툴바

        textView = (TextView) findViewById(R.id.confirm_version); // 객체 연결
        textView.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
            Intent i = new Intent(android.content.Intent.ACTION_VIEW);
                i.setData(Uri.parse("https://play.google.com/store/apps/details?id=com.sports2i.kbo"));
                startActivity(i);
            } //  암시적 인텐트를 활용해서 텍스트 클릭 시 해당 어플 구글플레이 화면으로 이동하게끔 지정
        });

        final String[] urls = {"https://play.google.com/store/apps/details?id=com.CheerSong",
        "https://play.google.com/store/apps/details?id=com.uplus.baseballhdtv",
        "https://play.google.com/store/apps/details?id=kr.co.redss.cleanbaseballcenter",
        "https://play.google.com/store/apps/details?id=com.zdivdev.youtube.kbo",
        "https://play.google.com/store/apps/details?id=com.yachin.app"
        }; // 리스트 뷰 구글 플레이 경로 배열
        final String[] mid = { "프로야구 응원가\n(구글플레이 스토어에서 확인하기)\n",
                "U+ 프로야구\n(구글플레이 스토어에서 확인하기)\n",
                "KBO클린베이스볼센터\n(구글플레이 스토어에서 확인하기)\n",
                "한국 프로야구 명장면 다시보기\n(구글플레이 스토어에서 확인하기)\n",
                "야구친구\n(구글플레이 스토어에서 확인하기)\n"
        }; // 리스트 뷰 텍스트 지정

        // 리스트 뷰 선언 및 구현
        ListView list_inf = (ListView) findViewById(R.id.listView_inf);
        ArrayAdapter<String> adapter = new ArrayAdapter<String>(this, android.R.layout.simple_list_item_1, mid);
        list_inf.setAdapter(adapter);

        int res = R.color.appbg; // 색상값을 저장해서 리스트 뷰의 배경색으로 지정
        list_inf.setBackgroundColor(getResources().getColor(res));

        list_inf.setOnItemClickListener(new AdapterView.OnItemClickListener() {
            @Override // 각각의 리스트 뷰에 따라 클릭 시 호출되는 구글플레이 경로를 지정
            public void onItemClick(AdapterView<?> arg0, View view, int position, long id) {
                Intent i = new Intent(android.content.Intent.ACTION_VIEW);
                i.setData(Uri.parse(urls[position])); // 배열에 지정된 url 설정
                startActivity(i);
            }
        });
    }
}