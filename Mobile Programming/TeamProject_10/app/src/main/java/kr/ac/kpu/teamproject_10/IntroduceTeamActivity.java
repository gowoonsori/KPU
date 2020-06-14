package kr.ac.kpu.teamproject_10;
import android.app.Activity;
import android.content.Intent;
import android.content.SharedPreferences;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.os.Bundle;
import android.view.LayoutInflater;
import android.view.MenuItem;
import android.view.View;
import android.view.ViewGroup;
import android.widget.AdapterView;
import android.widget.ArrayAdapter;
import android.widget.ImageView;
import android.widget.ListView;
import android.widget.RatingBar;
import android.widget.TextView;
import androidx.annotation.Nullable;
import androidx.appcompat.app.AppCompatActivity;
import java.io.ByteArrayOutputStream;
public class IntroduceTeamActivity extends AppCompatActivity {
    //해당 액티비티에서 사용할 객체 선언
    public static final String PREFS = "MyPrefs";
    ListView list; // 리스트뷰 변수 선언 및 목록
    String[] titles = {"두산 베어스", "키움 히어로즈", "SK 와이번스", "LG 트윈스",
    "NC 다이노스", "KT 위즈", "KIA 타이거즈", "삼성 라이온즈", "한화 이글스", "롯데 자이언츠"};

    float [] number ={0,0,0,0,0,0,0,0,0,0};
    CustomList adapter;
    Integer[] images = {R.drawable.logo_bears, R.drawable.logo_heroes,
            R.drawable.logo_wyverns, R.drawable.logo_twins,
            R.drawable.logo_dinos, R.drawable.logo_wiz,
            R.drawable.logo_tigers, R.drawable.logo_lions,
            R.drawable.logo_eagles, R.drawable.logo_giants};
    TextView rating;

    @Override
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
        setContentView(R.layout.activity_introduce_team);

        // 타이틀 바 설정(텍스트, 아이콘), 툴바 설정 | 색상은 styles.xml에서 작성
        setTitle(" 구단 소개");
        getSupportActionBar().setDisplayShowHomeEnabled(true);
        getSupportActionBar().setIcon(R.drawable.person);
        getSupportActionBar().setDisplayHomeAsUpEnabled(true); // 툴바

        //저장된 평점을 가져오기 위한 프레퍼런스
        SharedPreferences setting = getSharedPreferences(PREFS,0);
        for(int i=0; i<10; i++)
            number[i] = setting.getFloat("tmp["+i+"]", number[i]);

        // 커스텀 리스트에 어댑터 적용. 각 리스트를 클릭할 수 있는 onclick 메소드
        adapter = new CustomList(IntroduceTeamActivity.this);
        list=findViewById(R.id.list);
        list.setAdapter(adapter);
        list.setOnItemClickListener(new AdapterView.OnItemClickListener() {
            @Override
            public void onItemClick(AdapterView<?> parent, View view, int position, long id) {
                onclick(position);
            }
        });
    }
    //앱 종료시 수정된 평점을 저장하기 위한 프레퍼런스
    @Override
    protected void onStop() {
        super.onStop();
        SharedPreferences settings = getSharedPreferences(PREFS, 0);
        SharedPreferences.Editor editor = settings.edit();
        for(int i=0; i<10; i++)
            editor.putFloat("tmp["+i+"]", number[i]);
        editor.commit();
    }
    //커스텀 리스트 구현
    public class CustomList extends ArrayAdapter<String> {
        private final Activity context;
        public CustomList(Activity context) {
            super(context, R.layout.listview_intro_team, titles);
            this.context = context;
        }
        //각 리스트에 표시되는 객체들을 구현
        @Override
        public View getView(int position, View view, ViewGroup parent) {
            LayoutInflater inflater = context.getLayoutInflater();
            View rowView = inflater.inflate(R.layout.listview_intro_team, null);
            ImageView imageView = rowView.findViewById(R.id.image);
            TextView title = rowView.findViewById(R.id.title);
            rating = rowView.findViewById(R.id.rating);
            RatingBar star = rowView.findViewById(R.id.star);
            TextView genre = rowView.findViewById(R.id.home);
            title.setText(titles[position]);
            imageView.setImageResource(images[position]);
            rating.setText(""+number[position]);
            star.setRating(number[position]);
            genre.setText(""+(position+1));

            return rowView;
        }
    }
    // 각 리스트를 클릭하면 호출되는 onclick 메소드
    private void onclick(int position) {
        Intent intent =  new Intent(this, SelectedTeamActivity.class);
        intent.putExtra("title", titles[position]);
        Bitmap bitmap = BitmapFactory.decodeResource(getResources(), images[position]);
        ByteArrayOutputStream stream = new ByteArrayOutputStream();
        bitmap.compress(Bitmap.CompressFormat.PNG, 100, stream);
        byte[] byteArray = stream.toByteArray();
        intent.putExtra("image", byteArray);
        intent.putExtra("number",number[position]);
        startActivityForResult(intent,0);
    }
    // SelectedTeamActivity에서 설정한 별점과 점수를 표시
    @Override
    protected void onActivityResult(int requestCode, int resultCode, @Nullable Intent data) {
        if (resultCode == RESULT_OK) {
            for(int i=0; i<titles.length; i++) {
                number[i] = data.getFloatExtra(titles[i], number[i]);
                rating.setText(number[i] + "");
                adapter.notifyDataSetChanged();
            }
        }
    }
}