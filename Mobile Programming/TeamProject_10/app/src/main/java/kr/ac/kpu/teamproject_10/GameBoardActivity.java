package kr.ac.kpu.teamproject_10;
import android.content.Context;
import android.database.Cursor;
import android.database.sqlite.SQLiteDatabase;
import android.database.sqlite.SQLiteOpenHelper;
import android.graphics.Color;
import android.os.Bundle;
import android.view.MenuItem;
import android.view.View;
import android.widget.Button;
import android.widget.CalendarView;
import android.widget.ImageView;
import android.widget.ScrollView;
import android.widget.TextView;
import androidx.annotation.NonNull;
import androidx.appcompat.app.AlertDialog;
import androidx.appcompat.app.AppCompatActivity;
import java.util.Calendar;
public class GameBoardActivity extends AppCompatActivity {
    /*DB생성,조회를 위한 변수*/
    private final String dbName="GameDB";
    SQLiteDatabase sqlDB; scheduleDB myScheduleDB;

    /*CalendrView를 위한 변수*/
    CalendarView calendarView;
    int selectYear,selectMonth,selectDay;

    /*팀 순위 보기 (Alert Dialog)를 위한 변수*/
    Button rankingBt; View rankingView;

    /*db로부터 경기일정을 load해 view해줄 변수*/
    ImageView[][] imV=new ImageView[5][2];
    TextView[][] clubName=new TextView[5][2];
    TextView[][] Score=new TextView[5][2];
    TextView[] Stadium=new TextView[5];

    /*경기있는 날/없는 날 에 따라 다른 view를 보여주기 위함*/
    ScrollView scv; TextView noschedule;

    @Override // 툴바 뒤로가기 기능
    public boolean onOptionsItemSelected(MenuItem item) {
        switch (item.getItemId()){
            case android.R.id.home: // toolbar의 back키 눌렀을 때 동작
                finish(); return true; // 이전 액티비티로 회귀
        } return super.onOptionsItemSelected(item);
    }
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_game_board);

        // 타이틀 바 설정(텍스트, 아이콘), 툴바 설정 | 색상은 styles.xml에서 작성
        setTitle("경기 일정 및 결과");
        getSupportActionBar().setDisplayShowHomeEnabled(true);
        getSupportActionBar().setIcon(R.drawable.kbo_2020_logo);
        getSupportActionBar().setDisplayHomeAsUpEnabled(true); // 툴바

        scv=findViewById(R.id.ScrollV);
        noschedule=findViewById(R.id.noScheduleTv);

        /*구단 명*/
        clubName[0][0]=findViewById(R.id.match1_club1);
        clubName[0][1]=findViewById(R.id.match1_club2);
        clubName[1][0]=findViewById(R.id.match2_club1);
        clubName[1][1]=findViewById(R.id.match2_club2);
        clubName[2][0]=findViewById(R.id.match3_club1);
        clubName[2][1]=findViewById(R.id.match3_club2);
        clubName[3][0]=findViewById(R.id.match4_club1);
        clubName[3][1]=findViewById(R.id.match4_club2);
        clubName[4][0]=findViewById(R.id.match5_club1);
        clubName[4][1]=findViewById(R.id.match5_club2);
        /*점수*/
        Score[0][0]=findViewById(R.id.match1_score_tv1);
        Score[0][1]=findViewById(R.id.match1_score_tv2);
        Score[1][0]=findViewById(R.id.match2_score_tv1);
        Score[1][1]=findViewById(R.id.match2_score_tv2);
        Score[2][0]=findViewById(R.id.match3_score_tv1);
        Score[2][1]=findViewById(R.id.match3_score_tv2);
        Score[3][0]=findViewById(R.id.match4_score_tv1);
        Score[3][1]=findViewById(R.id.match4_score_tv2);
        Score[4][0]=findViewById(R.id.match5_score_tv1);
        Score[4][1]=findViewById(R.id.match5_score_tv2);
        Stadium[0]=findViewById(R.id.match1_stadium_tv);
        Stadium[1]=findViewById(R.id.match2_stadium_tv);
        Stadium[2]=findViewById(R.id.match3_stadium_tv);
        Stadium[3]=findViewById(R.id.match4_stadium_tv);
        Stadium[4]=findViewById(R.id.match5_stadium_tv);
        /*구단 이미지*/
        imV[0][0]=findViewById(R.id.match1_iv1);
        imV[0][1]=findViewById(R.id.match1_iv2);
        imV[1][0]=findViewById(R.id.match2_iv1);
        imV[1][1]=findViewById(R.id.match2_iv2);
        imV[2][0]=findViewById(R.id.match3_iv1);
        imV[2][1]=findViewById(R.id.match3_iv2);
        imV[3][0]=findViewById(R.id.match4_iv1);
        imV[3][1]=findViewById(R.id.match4_iv2);
        imV[4][0]=findViewById(R.id.match5_iv1);
        imV[4][1]=findViewById(R.id.match5_iv2);

        /*DB 연결하기 위한 변수*/
        myScheduleDB = new scheduleDB(this);
        /*오늘 날짜의 경기일정을 보여줌*/
        Calendar cal = Calendar.getInstance();
        int year =cal.get(Calendar.YEAR);
        int month = cal.get(Calendar.MONTH)+1;
        int day = cal.get(Calendar.DATE);

        setSceduleView(year,month,day);   //경기일정을 db에서 읽어와 스크롤 뷰 안에 set

        //캘린더 뷰 이벤트 리스너
        calendarView=(CalendarView)findViewById(R.id.Cv);
        calendarView.setOnDateChangeListener(new CalendarView.OnDateChangeListener() {
            @Override
            public void onSelectedDayChange(@NonNull CalendarView calendarView, int year, int month, int day) {
                /*경기일정 초기화 (Text,TextColor 등)*/
                scheduleInit();
                /*캘린더뷰를 통하여 선택한 날짜 */
                selectYear=year;
                selectMonth=month+1;
                selectDay=day;
                /*선택한 날짜의 경기일정을 db를 통해 set*/
                setSceduleView(selectYear,selectMonth,selectDay);
            }
        });
        /*팀 순위 보기 버튼 이벤트*/
        rankingBt=(Button)findViewById(R.id.teamRankingBt);
        rankingBt.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                rankingView= (View)View.inflate(GameBoardActivity.this,R.layout.dialog_game_board,null);
                AlertDialog.Builder dlg=new AlertDialog.Builder(GameBoardActivity.this);
                dlg.setTitle("전체 팀 순위");
                dlg.setView(rankingView);
                dlg.setPositiveButton("확인",null);
                dlg.show();
            }
        });
    }
    /*db로부터 해당 날짜의 경기일정을 읽어와 set해주는 함수*/
    public void setSceduleView(int year,int month,int day){
        /*날짜가 int값이며, db에는 day,month이 두자리수로 표기되어있는데 해당 값은 10의 미만자리면 한자리수 표기
        이기때문에 10보다 작게 되면 두자리수로 표현
        ex) db에는 2020/06/01 로 저장이 되어있는데 year/month/day는 2020/6/1 이기 때문에 0을 붙여준다. */
        String sDay;
        if(day<10)sDay="0"+Integer.toString(day);
        else sDay=Integer.toString(day);
        String sMonth="0"+Integer.toString(month);

        /*DB Read*/
        sqlDB=myScheduleDB.getReadableDatabase();
        Cursor cursor;
        cursor = sqlDB.rawQuery("SELECT * FROM ScheduleDB WHERE date = '"+Integer.toString(year)+"/"+sMonth+"/"+sDay+"';",null);

        int i=0; //TextView배열참조를 위한 변수
        /*커서가 끝날때 까지 값을 읽어와 set*/
        while(cursor.moveToNext()){
            /*ScrollView Visible로 바꾸고 TextView를 Gone으로 바꾼다.*/
            scv.setVisibility(View.VISIBLE);
            noschedule.setVisibility(View.GONE);

            clubName[i][0].setText(cursor.getString(1));
            setImageView(clubName[i][0].getText().toString(),i,0);
            Score[i][0].setText(cursor.getString(2));
            setScoreStyle(Score[i][0].getText().toString(),i,0);
            Score[i][1].setText(cursor.getString(3));
            setScoreStyle(Score[i][1].getText().toString(),i,1);
            clubName[i][1].setText(cursor.getString(4));
            setImageView(clubName[i][1].getText().toString(),i,1);
            Stadium[i].setText(cursor.getString(5));
            i++;
        }
        cursor.close();
        sqlDB.close();
    }
    /*해당 이름의 구단명에 맞는 사진을 ImageView에 set*/
    public void setImageView(String clubName,int i,int j){
        if(clubName.equals("두산"))
            imV[i][j].setImageResource(R.drawable.logo_bears);
        else if(clubName.equals("KIA"))
            imV[i][j].setImageResource(R.drawable.logo_tigers);
        else if(clubName.equals("한화"))
            imV[i][j].setImageResource(R.drawable.logo_eagles);
        else if(clubName.equals("SK"))
            imV[i][j].setImageResource(R.drawable.logo_wyverns);
        else if(clubName.equals("KT"))
            imV[i][j].setImageResource(R.drawable.logo_wiz);
        else if(clubName.equals("삼성"))
            imV[i][j].setImageResource(R.drawable.logo_lions);
        else if(clubName.equals("NC"))
            imV[i][j].setImageResource(R.drawable.logo_dinos);
        else if(clubName.equals("키움"))
            imV[i][j].setImageResource(R.drawable.logo_heroes);
        else if(clubName.equals("롯데"))
            imV[i][j].setImageResource(R.drawable.logo_giants);
        else if(clubName.equals("LG"))
            imV[i][j].setImageResource(R.drawable.logo_twins);
    }
    /*경기 일정 초기화 함수*/
    public void scheduleInit(){
        scv.setVisibility(View.GONE);
        noschedule.setVisibility(View.VISIBLE);
        /*TextView color 초기화*/
        for(int i=0;i<5;i++)
            for(int j=0;j<2;j++)
                Score[i][j].setTextColor(Color.parseColor("#4A4A4A"));
    }
    /*db에서 읽어온 점수가 NULL 즉 아직 경기를 치루지 않았다면 표시에는 공백으로 보이게 하고
    경기를 치뤘다면 점수를 계산하여 이긴팀의 점수는 빨간색으로 set */
    public void setScoreStyle(String score,int i,int j){
        if(score.equals("NULL"))
            Score[i][j].setText(" ");
        else if(j==1){
            if(Integer.parseInt(Score[i][0].getText().toString())>Integer.parseInt(Score[i][1].getText().toString()))
                Score[i][0].setTextColor(Color.parseColor("#FF0000"));
            else if(Integer.parseInt(Score[i][0].getText().toString())<Integer.parseInt(Score[i][1].getText().toString()))
                Score[i][1].setTextColor(Color.parseColor("#FF0000"));
        }
    }
    /*DB생성 클래스*/
    public class scheduleDB extends SQLiteOpenHelper {
        public scheduleDB(Context context){
            super(context,dbName,null,1);
        }
        @Override
        public void onCreate(SQLiteDatabase db) {
            db.execSQL("CREATE TABLE gameScheduleDB ( date TEXT,club1 TEXT,score1 TEXT,score2 TEXT,club2 TEXT,stadium TEXT);");
        }
        @Override
        public void onUpgrade(SQLiteDatabase db, int oldVersion, int newVerson) {
            db.execSQL("DROP TABLE IF EXISTS gameScheduleDB");
            onCreate(db);
        }
    }
}