package kr.ac.kpu.teamproject_10;
import android.content.DialogInterface;
import android.content.Intent;
import android.net.Uri;
import android.os.Bundle;
import android.view.Gravity;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.widget.AdapterView;
import android.widget.ArrayAdapter;
import android.widget.ImageView;
import android.widget.ListView;
import android.widget.Toast;

import androidx.appcompat.app.AlertDialog;
import androidx.appcompat.app.AppCompatActivity;
public class MainActivity extends AppCompatActivity {
    View dialogView; ImageView select; // 대화상자 용 객체

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        // 타이틀 바 수정구현
        setTitle("KBO 리그");
        getSupportActionBar().setDisplayShowHomeEnabled(true);
        getSupportActionBar().setIcon(R.drawable.kbo_2020_logo);

        // 리스트 뷰 선언 및 구현
        final String[] mid = {"리그 소개", "구단 소개", "경기 일정 및 결과"};
        ListView list_main = (ListView) findViewById(R.id.listView_main);
        ArrayAdapter<String> adapter = new ArrayAdapter<String>(this, android.R.layout.simple_list_item_1, mid);
        list_main.setAdapter(adapter);

        list_main.setOnItemClickListener(new AdapterView.OnItemClickListener() {
            @Override
            public void onItemClick(AdapterView<?> arg0, View arg1, int arg2, long arg3) {
                switch (arg2) { // 각각의 리스트 뷰에 따라 클릭 시 일어나는 이벤트를 달리 해주고 싶은데.....
                    case 0:
                        Intent intent1= new Intent(getApplicationContext(), IntroduceKBOActivity.class);
                        startActivity(intent1);
                        break;
                    case 1:
                        Intent intent2 = new Intent(getApplicationContext(), IntroduceTeamActivity.class);
                        startActivity(intent2);
                        break;
                    case 2:
                        Intent intent3 = new Intent(getApplicationContext(), GameBoardActivity.class);
                        startActivity(intent3);
                        break;

                }
            }
        });

        // 아래 대화상자 각 팀에 대한 객체 생성 및 연결
        final ImageView t1, t2, t3, t4, t5, t6, t7, t8, t9, t10;
        t1 = (ImageView) findViewById(R.id.t1); t2 = (ImageView) findViewById(R.id.t2);
        t3 = (ImageView) findViewById(R.id.t3); t4 = (ImageView) findViewById(R.id.t4);
        t5 = (ImageView) findViewById(R.id.t5); t6 = (ImageView) findViewById(R.id.t6);
        t7 = (ImageView) findViewById(R.id.t7); t8 = (ImageView) findViewById(R.id.t8);
        t9 = (ImageView) findViewById(R.id.t9); t10 = (ImageView) findViewById(R.id.t10);

        // 각 이미지 버튼 클릭 시 대화상자 생성(10개나... 코드 길다...!)
        t1.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                dialogView = (View) View.inflate(MainActivity.this, R.layout.dialog_main, null);
                select = (ImageView) dialogView.findViewById(R.id.select);
                AlertDialog.Builder dlg1 = new AlertDialog.Builder(MainActivity.this);
                select.setImageResource(R.drawable.uniform_bears);
                dlg1.setTitle("두산 베어스");
                dlg1.setIcon(R.drawable.logo_bears);
                dlg1.setView(dialogView);
                dlg1.setPositiveButton("닫기", new DialogInterface.OnClickListener() {
                    @Override
                    public void onClick(DialogInterface dialog, int which) {
                        Toast toast = Toast.makeText(MainActivity.this, "두산 베어스 홈/원정 유니폼입니다.", Toast.LENGTH_SHORT);
                        toast.setGravity(Gravity.BOTTOM, 0, 300); toast.show();
                        // Toast.makeText(MainActivity.this, "두산 베어스 홈/원정 유니폼입니다.", Toast.LENGTH_SHORT).show();
                    }
                });
                dlg1.setNegativeButton("쇼핑몰 이동", new DialogInterface.OnClickListener() {
                    @Override
                    public void onClick(DialogInterface dialog, int which) {
                        startActivity(new Intent(Intent.ACTION_VIEW, Uri.parse("https://www.doosanbearswefan.shop:14014/shop/main/index.php")));
                    }
                });
                dlg1.show();
            }
        });
        t2.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                dialogView = (View) View.inflate(MainActivity.this, R.layout.dialog_main, null);
                select = (ImageView) dialogView.findViewById(R.id.select);
                AlertDialog.Builder dlg2 = new AlertDialog.Builder(MainActivity.this);
                select.setImageResource(R.drawable.uniform_heroes);
                dlg2.setTitle("키움 히어로즈");
                dlg2.setIcon(R.drawable.logo_heroes);
                dlg2.setView(dialogView);
                dlg2.setPositiveButton("닫기", new DialogInterface.OnClickListener() {
                    @Override
                    public void onClick(DialogInterface dialog, int which) {
                        Toast toast = Toast.makeText(MainActivity.this, "키움 히어로즈 홈/원정 유니폼입니다.", Toast.LENGTH_SHORT);
                        toast.setGravity(Gravity.BOTTOM, 0, 300); toast.show();
                        // Toast.makeText(MainActivity.this, "키움 히어로즈 홈/원정 유니폼입니다.", Toast.LENGTH_SHORT).show();
                    }
                });
                dlg2.setNegativeButton("쇼핑몰 이동", new DialogInterface.OnClickListener() {
                    @Override
                    public void onClick(DialogInterface dialog, int which) {
                        startActivity(new Intent(Intent.ACTION_VIEW, Uri.parse("http://ticket.interpark.com/contents/Promotion/MDShopList?dispNo=001780002001")));
                    }
                });
                dlg2.show();
            }
        });
        t3.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                dialogView = (View) View.inflate(MainActivity.this, R.layout.dialog_main, null);
                select = (ImageView) dialogView.findViewById(R.id.select);
                AlertDialog.Builder dlg3 = new AlertDialog.Builder(MainActivity.this);
                select.setImageResource(R.drawable.uniform_wyverns);
                dlg3.setTitle("SK 와이번스");
                dlg3.setIcon(R.drawable.logo_wyverns);
                dlg3.setView(dialogView);
                dlg3.setPositiveButton("닫기", new DialogInterface.OnClickListener() {
                    @Override
                    public void onClick(DialogInterface dialog, int which) {
                        Toast toast = Toast.makeText(MainActivity.this, "SK 와이번스 홈/원정 유니폼입니다.", Toast.LENGTH_SHORT);
                        toast.setGravity(Gravity.BOTTOM, 0, 300); toast.show();
                        // Toast.makeText(MainActivity.this, "SK 와이번스 홈/원정 유니폼입니다.", Toast.LENGTH_SHORT).show();
                    }
                });
                dlg3.setNegativeButton("쇼핑몰 이동", new DialogInterface.OnClickListener() {
                    @Override
                    public void onClick(DialogInterface dialog, int which) {
                        startActivity(new Intent(Intent.ACTION_VIEW, Uri.parse("http://www.wyvernsshop.co.kr/")));
                    }
                });
                dlg3.show();
            }
        });
        t4.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                dialogView = (View) View.inflate(MainActivity.this, R.layout.dialog_main, null);
                select = (ImageView) dialogView.findViewById(R.id.select);
                AlertDialog.Builder dlg4 = new AlertDialog.Builder(MainActivity.this);
                select.setImageResource(R.drawable.uniform_twins);
                dlg4.setTitle("LG 트윈스");
                dlg4.setIcon(R.drawable.logo_twins);
                dlg4.setView(dialogView);
                dlg4.setPositiveButton("닫기", new DialogInterface.OnClickListener() {
                    @Override
                    public void onClick(DialogInterface dialog, int which) {
                        Toast toast = Toast.makeText(MainActivity.this, "LG 트윈스 홈/원정 유니폼입니다.", Toast.LENGTH_SHORT);
                        toast.setGravity(Gravity.BOTTOM, 0, 300); toast.show();
                        // Toast.makeText(MainActivity.this, "LG 트윈스 홈/원정 유니폼입니다.", Toast.LENGTH_SHORT).show();
                    }
                });
                dlg4.setNegativeButton("쇼핑몰 이동", new DialogInterface.OnClickListener() {
                    @Override
                    public void onClick(DialogInterface dialog, int which) {
                        startActivity(new Intent(Intent.ACTION_VIEW, Uri.parse("http://twins.lockerdium.co.kr/")));
                    }
                });

                dlg4.show();
            }
        });
        t5.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                dialogView = (View) View.inflate(MainActivity.this, R.layout.dialog_main, null);
                select = (ImageView) dialogView.findViewById(R.id.select);
                AlertDialog.Builder dlg5 = new AlertDialog.Builder(MainActivity.this);
                select.setImageResource(R.drawable.uniform_dinos);
                dlg5.setTitle("NC 다이노스");
                dlg5.setIcon(R.drawable.logo_dinos);
                dlg5.setView(dialogView);
                dlg5.setPositiveButton("닫기", new DialogInterface.OnClickListener() {
                    @Override
                    public void onClick(DialogInterface dialog, int which) {
                        Toast toast = Toast.makeText(MainActivity.this, "NC 다이노스 홈/원정 유니폼입니다.", Toast.LENGTH_SHORT);
                        toast.setGravity(Gravity.BOTTOM, 0, 300); toast.show();
                        // Toast.makeText(MainActivity.this, "NC 다이노스 홈/원정 유니폼입니다.", Toast.LENGTH_SHORT).show();
                    }
                });
                dlg5.setNegativeButton("쇼핑몰 이동", new DialogInterface.OnClickListener() {
                    @Override
                    public void onClick(DialogInterface dialog, int which) {
                        startActivity(new Intent(Intent.ACTION_VIEW, Uri.parse("http://ncdinos.lockerdium.co.kr/goods/goods_list.php?cateCd=001001")));
                    }
                });
                dlg5.show();
            }
        });
        t6.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                dialogView = (View) View.inflate(MainActivity.this, R.layout.dialog_main, null);
                select = (ImageView) dialogView.findViewById(R.id.select);
                AlertDialog.Builder dlg6 = new AlertDialog.Builder(MainActivity.this);
                select.setImageResource(R.drawable.uniform_wiz);
                dlg6.setTitle("KT 위즈");
                dlg6.setIcon(R.drawable.logo_wiz);
                dlg6.setView(dialogView);
                dlg6.setPositiveButton("닫기", new DialogInterface.OnClickListener() {
                    @Override
                    public void onClick(DialogInterface dialog, int which) {
                        Toast toast = Toast.makeText(MainActivity.this, "KT 위즈 홈/원정 유니폼입니다.", Toast.LENGTH_SHORT);
                        toast.setGravity(Gravity.BOTTOM, 0, 300); toast.show();
                        // Toast.makeText(MainActivity.this, "KT 위즈 홈/원정 유니폼입니다.", Toast.LENGTH_SHORT).show();
                    }
                });
                dlg6.setNegativeButton("쇼핑몰 이동", new DialogInterface.OnClickListener() {
                    @Override
                    public void onClick(DialogInterface dialog, int which) {
                        startActivity(new Intent(Intent.ACTION_VIEW, Uri.parse("https://www.ktwiz.shop:14046/shop/main/index.php")));
                    }
                });
                dlg6.show();
            }
        });
        t7.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                dialogView = (View) View.inflate(MainActivity.this, R.layout.dialog_main, null);
                select = (ImageView) dialogView.findViewById(R.id.select);
                AlertDialog.Builder dlg7 = new AlertDialog.Builder(MainActivity.this);
                select.setImageResource(R.drawable.uniform_tigers);
                dlg7.setTitle("KIA 타이거즈");
                dlg7.setIcon(R.drawable.logo_tigers);
                dlg7.setView(dialogView);
                dlg7.setPositiveButton("닫기", new DialogInterface.OnClickListener() {
                    @Override
                    public void onClick(DialogInterface dialog, int which) {
                        Toast toast = Toast.makeText(MainActivity.this, "KIA 타이거즈 홈/원정 유니폼입니다.", Toast.LENGTH_SHORT);
                        toast.setGravity(Gravity.BOTTOM, 0, 300); toast.show();
                        // Toast.makeText(MainActivity.this, "KIA 타이거즈 홈/원정 유니폼입니다.", Toast.LENGTH_SHORT).show();
                    }
                });
                dlg7.setNegativeButton("쇼핑몰 이동", new DialogInterface.OnClickListener() {
                    @Override
                    public void onClick(DialogInterface dialog, int which) {
                        startActivity(new Intent(Intent.ACTION_VIEW, Uri.parse("http://www.tigersshop.kr/")));
                    }
                });
                dlg7.show();
            }
        });
        t8.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                dialogView = (View) View.inflate(MainActivity.this, R.layout.dialog_main, null);
                select = (ImageView) dialogView.findViewById(R.id.select);
                AlertDialog.Builder dlg8 = new AlertDialog.Builder(MainActivity.this);
                select.setImageResource(R.drawable.uniform_lions);
                dlg8.setTitle("삼성 라이온즈");
                dlg8.setIcon(R.drawable.logo_lions);
                dlg8.setView(dialogView);
                dlg8.setPositiveButton("닫기", new DialogInterface.OnClickListener() {
                    @Override
                    public void onClick(DialogInterface dialog, int which) {
                        Toast toast = Toast.makeText(MainActivity.this, "삼성 라이온즈 홈/원정 유니폼입니다.", Toast.LENGTH_SHORT);
                        toast.setGravity(Gravity.BOTTOM, 0, 300); toast.show();
                        // Toast.makeText(MainActivity.this, "삼성 라이온즈 홈/원정 유니폼입니다.", Toast.LENGTH_SHORT).show();
                    }
                });
                dlg8.setNegativeButton("쇼핑몰 이동", new DialogInterface.OnClickListener() {
                    @Override
                    public void onClick(DialogInterface dialog, int which) {
                        startActivity(new Intent(Intent.ACTION_VIEW, Uri.parse("http://www.samsunglionsshop.com/")));
                    }
                });
                dlg8.show();
            }
        });
        t9.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                dialogView = (View) View.inflate(MainActivity.this, R.layout.dialog_main, null);
                select = (ImageView) dialogView.findViewById(R.id.select);
                AlertDialog.Builder dlg9 = new AlertDialog.Builder(MainActivity.this);
                select.setImageResource(R.drawable.uniform_eagles);
                dlg9.setTitle("한화 이글스");
                dlg9.setIcon(R.drawable.logo_eagles);
                dlg9.setView(dialogView);
                dlg9.setPositiveButton("닫기", new DialogInterface.OnClickListener() {
                    @Override
                    public void onClick(DialogInterface dialog, int which) {
                        Toast toast = Toast.makeText(MainActivity.this, "한화 이글스 홈/원정 유니폼입니다.", Toast.LENGTH_SHORT);
                        toast.setGravity(Gravity.BOTTOM, 0, 300); toast.show();
                        // Toast.makeText(MainActivity.this, "한화 이글스 홈/원정 유니폼입니다.", Toast.LENGTH_SHORT).show();
                    }
                });
                dlg9.setNegativeButton("쇼핑몰 이동", new DialogInterface.OnClickListener() {
                    @Override
                    public void onClick(DialogInterface dialog, int which) {
                        startActivity(new Intent(Intent.ACTION_VIEW, Uri.parse("http://eagles.lockerdium.co.kr/")));
                    }
                });
                dlg9.show();
            }
        });
        t10.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                dialogView = (View) View.inflate(MainActivity.this, R.layout.dialog_main, null);
                select = (ImageView) dialogView.findViewById(R.id.select);
                AlertDialog.Builder dlg10 = new AlertDialog.Builder(MainActivity.this);
                select.setImageResource(R.drawable.uniform_giants);
                dlg10.setTitle("롯데 자이언츠");
                dlg10.setIcon(R.drawable.logo_giants);
                dlg10.setView(dialogView);
                dlg10.setPositiveButton("닫기", new DialogInterface.OnClickListener() {
                    @Override
                    public void onClick(DialogInterface dialog, int which) {
                        Toast toast = Toast.makeText(MainActivity.this, "롯데 자이언츠 홈/원정 유니폼입니다.", Toast.LENGTH_SHORT);
                        toast.setGravity(Gravity.BOTTOM, 0, 300); toast.show();
                        // Toast.makeText(MainActivity.this, "롯데 자이언츠 홈/원정 유니폼입니다.", Toast.LENGTH_SHORT).show();
                    }
                });
                dlg10.setNegativeButton("쇼핑몰 이동", new DialogInterface.OnClickListener() {
                    @Override
                    public void onClick(DialogInterface dialog, int which) {
                        startActivity(new Intent(Intent.ACTION_VIEW, Uri.parse("http://www.lottegiantsshop.com/")));
                    }
                });
                dlg10.show();
            }
        });
    }
    // 옵션 메뉴 생성 및 선택 리스너 구현
    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        getMenuInflater().inflate(R.menu.main_menu, menu);
        return true;
    }
    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        switch (item.getItemId()) {
            case R.id.information:
                startActivity(new Intent(this, Information.class));
                return true;
            default:
                return super.onOptionsItemSelected(item);
        }
    }
}