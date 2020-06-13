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
import android.widget.Toast;

import androidx.annotation.Nullable;
import androidx.appcompat.app.AppCompatActivity;

import java.io.ByteArrayOutputStream;

public class IntroduceTeamActivity extends AppCompatActivity {
    public static final String PREFS = "MyPrefs";
    ListView list;
    String[] titles = {"두산 베어스", "키움 히어로즈", "SK 와이번스",
            "LG 트윈스", "NC 다이노스", "KT 위즈", "KIA 타이거즈",
            "삼성 라이온즈", "한화 이글스", "롯데 자이언츠"};

    float [] number ={0,0,0,0,0,0,0,0,0,0};
    //RatingBar []star;
    CustomList adapter;
    Integer[] images = {R.drawable.logo_bears, R.drawable.logo_heroes,
            R.drawable.logo_wyverns, R.drawable.logo_twins,
            R.drawable.logo_dinos, R.drawable.logo_wiz,
            R.drawable.logo_tigers, R.drawable.logo_lions,
            R.drawable.logo_eagles, R.drawable.logo_giants};
    TextView rating;
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_introduce_team);

        // 타이틀 바 수정구현
        setTitle("구단 소개");
        getSupportActionBar().setDisplayHomeAsUpEnabled(true);
        getSupportActionBar().setDisplayShowHomeEnabled(true);
        getSupportActionBar().setIcon(R.drawable.kbo_2020_logo);

        SharedPreferences setting = getSharedPreferences(PREFS,0);
        for(int i=0; i<10; i++) {
            number[i] = setting.getFloat("tmp["+i+"]", number[i]);
        }

      /*  Intent intent = getIntent();
        for(int i=0; i<titles.length; i++){
            number[i]=intent.getFloatExtra(titles[i],number[i]);
        }
*/
        adapter = new CustomList(IntroduceTeamActivity.this);
        list=findViewById(R.id.list);
        list.setAdapter(adapter);
        list.setOnItemClickListener(new AdapterView.OnItemClickListener() {
            @Override
            public void onItemClick(AdapterView<?> parent, View view, int position, long id) {
                onclick(position);
                Toast.makeText(getBaseContext(), titles[position], Toast.LENGTH_SHORT).show();
            }
        });

    }

    @Override
    protected void onStop() {
        super.onStop();
        SharedPreferences settings = getSharedPreferences(PREFS, 0);
        SharedPreferences.Editor editor = settings.edit();
        for(int i=0; i<10; i++) {
            editor.putFloat("tmp["+i+"]", number[i]);
        }
        editor.commit();
    }

    public class CustomList extends ArrayAdapter<String> {
        private final Activity context;
        public CustomList(Activity context) {
            super(context, R.layout.listview_intro_team, titles);
            this.context = context;
        }

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
        //startActivity(intent);
    }

    @Override
    protected void onActivityResult(int requestCode, int resultCode, @Nullable Intent data) {

        if (resultCode == RESULT_OK) {
            for(int i=0; i<titles.length; i++) {
                number[i] = data.getFloatExtra(titles[i], number[i]);
                rating.setText(number[i] + ""); //여기에 별점 평균값 출력하기.
                adapter.notifyDataSetChanged();
            }
        }
    }
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
