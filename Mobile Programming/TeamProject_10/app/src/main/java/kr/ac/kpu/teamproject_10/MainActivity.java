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

    View dialogView; ImageView select; // 대화상자에 활용되는 객체

    @Override // 옵션 메뉴 생성 및 선택 리스너 구현
    public boolean onCreateOptionsMenu(Menu menu) {
        getMenuInflater().inflate(R.menu.main_menu, menu);
        return true;
    }
    @Override // 옵션 메뉴 선택 시 실행되는 액티비티 지정
    public boolean onOptionsItemSelected(MenuItem item) {
        switch (item.getItemId()) {
            case R.id.information:
                startActivity(new Intent(this, Information.class));
                return true;
            case R.id.call:
                Intent intent = new Intent(Intent.ACTION_VIEW, Uri.parse("tel:/02-3460-4600"));
                startActivity(intent);
                return true;
            default:
                return super.onOptionsItemSelected(item);
        }
    }
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        // 타이틀 바 설정(텍스트, 아이콘) | 색상은 styles.xml에서 작성
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
            public void onItemClick(AdapterView<?> arg0, View view, int position, long id) {
                switch (position) { // 각각의 리스트 뷰에 따라 클릭 시 호출되는 액티비티를 지정
                    case 0:
                        Intent intent1= new Intent(getApplicationContext(), IntroduceKBOActivity.class);
                        startActivity(intent1); break;
                    case 1:
                        Intent intent2 = new Intent(getApplicationContext(), IntroduceTeamActivity.class);
                        startActivity(intent2); break;
                    case 2:
                        Intent intent3 = new Intent(getApplicationContext(), GameBoardActivity.class);
                        startActivity(intent3); break;
                }
            }
        });
        // 각 팀에 대한 대화상자 객체 생성 및 연결
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
                Toast toast1 = Toast.makeText(MainActivity.this, "두산 베어스 홈/원정 유니폼입니다.", Toast.LENGTH_SHORT);
                toast1.setGravity(Gravity.BOTTOM, 0, 300); toast1.show(); // (대화상자 열릴때)구단로고와 텍스트가 겹치지 않도록 위치 조정

                dialogView = (View) View.inflate(MainActivity.this, R.layout.dialog_main, null);
                // dialog_main.xml 파일을 인플레이트 해서 dialogView에 대입
                select = (ImageView) dialogView.findViewById(R.id.select);
                // 대화상자의 이미지 뷰에 접근

                AlertDialog.Builder dlg1 = new AlertDialog.Builder(MainActivity.this);
                select.setImageResource(R.drawable.uniform_bears); // 이미지 뷰에 두산 유니폼 이미지 접목

                dlg1.setTitle("두산 베어스"); // 제목 설정
                dlg1.setIcon(R.drawable.logo_bears); // 아이콘 설정
                dlg1.setView(dialogView); // 인플레이트 한 뷰를 대화상자로 설정
                dlg1.setPositiveButton("닫기", new DialogInterface.OnClickListener() {
                    @Override // 닫기 버튼 클릭시 실행되는 리스너
                    public void onClick(DialogInterface dialog, int which) {
                        Toast toast2 = Toast.makeText(MainActivity.this, "대화 상자를 종료합니다.", Toast.LENGTH_SHORT);
                        toast2.setGravity(Gravity.BOTTOM, 0, 300); toast2.show(); // (대화상자 닫힐때)구단로고와 텍스트가 겹치지 않도록 위치 조정
                    }
                });
                dlg1.setNegativeButton("쇼핑몰 이동", new DialogInterface.OnClickListener() {
                    @Override // 쇼핑몰 이동 버튼 클릭시 실행되는 리스너
                    public void onClick(DialogInterface dialog, int which) {
                        startActivity(new Intent(Intent.ACTION_VIEW, Uri.parse("https://www.doosanbearswefan.shop:14014/shop/main/index.php")));
                    } // 구단 공식 샵으로 이동
                });
                dlg1.show();
            }
        });
        t2.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                Toast toast1 = Toast.makeText(MainActivity.this, "키움 히어로즈 홈/원정 유니폼입니다.", Toast.LENGTH_SHORT);
                toast1.setGravity(Gravity.BOTTOM, 0, 300); toast1.show(); // (대화상자 열릴때)구단로고와 텍스트가 겹치지 않도록 위치 조정

                dialogView = (View) View.inflate(MainActivity.this, R.layout.dialog_main, null);
                // dialog_main.xml 파일을 인플레이트 해서 dialogView에 대입
                select = (ImageView) dialogView.findViewById(R.id.select);
                // 대화상자의 이미지 뷰에 접근

                AlertDialog.Builder dlg2 = new AlertDialog.Builder(MainActivity.this);
                select.setImageResource(R.drawable.uniform_heroes); // 이미지 뷰에 키움 유니폼 이미지 접목

                dlg2.setTitle("키움 히어로즈"); // 제목 설정
                dlg2.setIcon(R.drawable.logo_heroes); // 아이콘 설정
                dlg2.setView(dialogView); // 인플레이트 한 뷰를 대화상자로 설정
                dlg2.setPositiveButton("닫기", new DialogInterface.OnClickListener() {
                    @Override // 닫기 버튼 클릭시 실행되는 리스너
                    public void onClick(DialogInterface dialog, int which) {
                        Toast toast2 = Toast.makeText(MainActivity.this, "대화 상자를 종료합니다.", Toast.LENGTH_SHORT);
                        toast2.setGravity(Gravity.BOTTOM, 0, 300); toast2.show(); // (대화상자 닫힐때)구단로고와 텍스트가 겹치지 않도록 위치 조정
                    }
                });
                dlg2.setNegativeButton("쇼핑몰 이동", new DialogInterface.OnClickListener() {
                    @Override // 쇼핑몰 이동 버튼 클릭시 실행되는 리스너
                    public void onClick(DialogInterface dialog, int which) {
                        startActivity(new Intent(Intent.ACTION_VIEW, Uri.parse("http://ticket.interpark.com/contents/Promotion/MDShopList?dispNo=001780002001")));
                    } // 구단 공식 샵으로 이동
                });
                dlg2.show();
            }
        });
        t3.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                Toast toast1 = Toast.makeText(MainActivity.this, "SK 와이번스 홈/원정 유니폼입니다.", Toast.LENGTH_SHORT);
                toast1.setGravity(Gravity.BOTTOM, 0, 300); toast1.show(); // (대화상자 열릴때)구단로고와 텍스트가 겹치지 않도록 위치 조정

                dialogView = (View) View.inflate(MainActivity.this, R.layout.dialog_main, null);
                // dialog_main.xml 파일을 인플레이트 해서 dialogView에 대입
                select = (ImageView) dialogView.findViewById(R.id.select);
                // 대화상자의 이미지 뷰에 접근

                AlertDialog.Builder dlg3 = new AlertDialog.Builder(MainActivity.this);
                select.setImageResource(R.drawable.uniform_wyverns); // 이미지 뷰에 SK 유니폼 이미지 접목

                dlg3.setTitle("SK 와이번스"); // 제목 설정
                dlg3.setIcon(R.drawable.logo_wyverns); // 아이콘 설정
                dlg3.setView(dialogView); // 인플레이트 한 뷰를 대화상자로 설정
                dlg3.setPositiveButton("닫기", new DialogInterface.OnClickListener() {
                    @Override // 닫기 버튼 클릭시 실행되는 리스너
                    public void onClick(DialogInterface dialog, int which) {
                        Toast toast2 = Toast.makeText(MainActivity.this, "대화 상자를 종료합니다.", Toast.LENGTH_SHORT);
                        toast2.setGravity(Gravity.BOTTOM, 0, 300); toast2.show(); // (대화상자 닫힐때)구단로고와 텍스트가 겹치지 않도록 위치 조정
                    }
                });
                dlg3.setNegativeButton("쇼핑몰 이동", new DialogInterface.OnClickListener() {
                    @Override // 쇼핑몰 이동 버튼 클릭시 실행되는 리스너
                    public void onClick(DialogInterface dialog, int which) {
                        startActivity(new Intent(Intent.ACTION_VIEW, Uri.parse("http://www.wyvernsshop.co.kr/")));
                    } // 구단 공식 샵으로 이동
                });
                dlg3.show();
            }
        });
        t4.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                Toast toast1 = Toast.makeText(MainActivity.this, "LG 트윈스 홈/원정 유니폼입니다.", Toast.LENGTH_SHORT);
                toast1.setGravity(Gravity.BOTTOM, 0, 300); toast1.show(); // (대화상자 열릴때)구단로고와 텍스트가 겹치지 않도록 위치 조정

                dialogView = (View) View.inflate(MainActivity.this, R.layout.dialog_main, null);
                // dialog_main.xml 파일을 인플레이트 해서 dialogView에 대입
                select = (ImageView) dialogView.findViewById(R.id.select);
                // 대화상자의 이미지 뷰에 접근

                AlertDialog.Builder dlg4 = new AlertDialog.Builder(MainActivity.this);
                select.setImageResource(R.drawable.uniform_twins); // 이미지 뷰에 LG 유니폼 이미지 접목

                dlg4.setTitle("LG 트윈스"); // 제목 설정
                dlg4.setIcon(R.drawable.logo_twins); // 아이콘 설정
                dlg4.setView(dialogView); // 인플레이트 한 뷰를 대화상자로 설정
                dlg4.setPositiveButton("닫기", new DialogInterface.OnClickListener() {
                    @Override // 닫기 버튼 클릭시 실행되는 리스너
                    public void onClick(DialogInterface dialog, int which) {
                        Toast toast2 = Toast.makeText(MainActivity.this, "대화 상자를 종료합니다.", Toast.LENGTH_SHORT);
                        toast2.setGravity(Gravity.BOTTOM, 0, 300); toast2.show(); // (대화상자 닫힐때)구단로고와 텍스트가 겹치지 않도록 위치 조정
                    }
                });
                dlg4.setNegativeButton("쇼핑몰 이동", new DialogInterface.OnClickListener() {
                    @Override // 쇼핑몰 이동 버튼 클릭시 실행되는 리스너
                    public void onClick(DialogInterface dialog, int which) {
                        startActivity(new Intent(Intent.ACTION_VIEW, Uri.parse("http://twins.lockerdium.co.kr/")));
                    } // 구단 공식 샵으로 이동
                });

                dlg4.show();
            }
        });
        t5.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                Toast toast1 = Toast.makeText(MainActivity.this, "NC 다이노스 홈/원정 유니폼입니다.", Toast.LENGTH_SHORT);
                toast1.setGravity(Gravity.BOTTOM, 0, 300); toast1.show(); // (대화상자 열릴때)구단로고와 텍스트가 겹치지 않도록 위치 조정

                dialogView = (View) View.inflate(MainActivity.this, R.layout.dialog_main, null);
                // dialog_main.xml 파일을 인플레이트 해서 dialogView에 대입
                select = (ImageView) dialogView.findViewById(R.id.select);
                // 대화상자의 이미지 뷰에 접근

                AlertDialog.Builder dlg5 = new AlertDialog.Builder(MainActivity.this);
                select.setImageResource(R.drawable.uniform_dinos); // 이미지 뷰에 NC 유니폼 이미지 접목

                dlg5.setTitle("NC 다이노스"); // 제목 설정
                dlg5.setIcon(R.drawable.logo_dinos); // 아이콘 설정
                dlg5.setView(dialogView); // 인플레이트 한 뷰를 대화상자로 설정
                dlg5.setPositiveButton("닫기", new DialogInterface.OnClickListener() {
                    @Override // 닫기 버튼 클릭시 실행되는 리스너
                    public void onClick(DialogInterface dialog, int which) {
                        Toast toast2 = Toast.makeText(MainActivity.this, "대화 상자를 종료합니다.", Toast.LENGTH_SHORT);
                        toast2.setGravity(Gravity.BOTTOM, 0, 300); toast2.show(); // (대화상자 닫힐때)구단로고와 텍스트가 겹치지 않도록 위치 조정
                    }
                });
                dlg5.setNegativeButton("쇼핑몰 이동", new DialogInterface.OnClickListener() {
                    @Override // 쇼핑몰 이동 버튼 클릭시 실행되는 리스너
                    public void onClick(DialogInterface dialog, int which) {
                        startActivity(new Intent(Intent.ACTION_VIEW, Uri.parse("http://ncdinos.lockerdium.co.kr/goods/goods_list.php?cateCd=001001")));
                    } // 구단 공식 샵으로 이동
                });
                dlg5.show();
            }
        });
        t6.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                Toast toast1 = Toast.makeText(MainActivity.this, "KT 위즈 홈/원정 유니폼입니다.", Toast.LENGTH_SHORT);
                toast1.setGravity(Gravity.BOTTOM, 0, 300); toast1.show(); // (대화상자 열릴때)구단로고와 텍스트가 겹치지 않도록 위치 조정

                dialogView = (View) View.inflate(MainActivity.this, R.layout.dialog_main, null);
                // dialog_main.xml 파일을 인플레이트 해서 dialogView에 대입
                select = (ImageView) dialogView.findViewById(R.id.select);
                // 대화상자의 이미지 뷰에 접근

                AlertDialog.Builder dlg6 = new AlertDialog.Builder(MainActivity.this);
                select.setImageResource(R.drawable.uniform_wiz); // 이미지 뷰에 KT 유니폼 이미지 접목

                dlg6.setTitle("KT 위즈"); // 제목 설정
                dlg6.setIcon(R.drawable.logo_wiz); // 아이콘 설정
                dlg6.setView(dialogView); // 인플레이트 한 뷰를 대화상자로 설정
                dlg6.setPositiveButton("닫기", new DialogInterface.OnClickListener() {
                    @Override // 닫기 버튼 클릭시 실행되는 리스너
                    public void onClick(DialogInterface dialog, int which) {
                        Toast toast2 = Toast.makeText(MainActivity.this, "대화 상자를 종료합니다.", Toast.LENGTH_SHORT);
                        toast2.setGravity(Gravity.BOTTOM, 0, 300); toast2.show(); // (대화상자 닫힐때)구단로고와 텍스트가 겹치지 않도록 위치 조정
                    }
                });
                dlg6.setNegativeButton("쇼핑몰 이동", new DialogInterface.OnClickListener() {
                    @Override // 쇼핑몰 이동 버튼 클릭시 실행되는 리스너
                    public void onClick(DialogInterface dialog, int which) {
                        startActivity(new Intent(Intent.ACTION_VIEW, Uri.parse("https://www.ktwiz.shop:14046/shop/main/index.php")));
                    } // 구단 공식 샵으로 이동
                });
                dlg6.show();
            }
        });
        t7.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                Toast toast1 = Toast.makeText(MainActivity.this, "KIA 타이거즈 홈/원정 유니폼입니다.", Toast.LENGTH_SHORT);
                toast1.setGravity(Gravity.BOTTOM, 0, 300); toast1.show(); // (대화상자 열릴때)구단로고와 텍스트가 겹치지 않도록 위치 조정

                dialogView = (View) View.inflate(MainActivity.this, R.layout.dialog_main, null);
                // dialog_main.xml 파일을 인플레이트 해서 dialogView에 대입
                select = (ImageView) dialogView.findViewById(R.id.select);
                // 대화상자의 이미지 뷰에 접근

                AlertDialog.Builder dlg7 = new AlertDialog.Builder(MainActivity.this);
                select.setImageResource(R.drawable.uniform_tigers); // 이미지 뷰에 KIA 유니폼 이미지 접목

                dlg7.setTitle("KIA 타이거즈"); // 제목 설정
                dlg7.setIcon(R.drawable.logo_tigers); // 아이콘 설정
                dlg7.setView(dialogView); // 인플레이트 한 뷰를 대화상자로 설정
                dlg7.setPositiveButton("닫기", new DialogInterface.OnClickListener() {
                    @Override // 닫기 버튼 클릭시 실행되는 리스너
                    public void onClick(DialogInterface dialog, int which) {
                        Toast toast2 = Toast.makeText(MainActivity.this, "대화 상자를 종료합니다.", Toast.LENGTH_SHORT);
                        toast2.setGravity(Gravity.BOTTOM, 0, 300); toast2.show(); // (대화상자 닫힐때)구단로고와 텍스트가 겹치지 않도록 위치 조정
                    }
                });
                dlg7.setNegativeButton("쇼핑몰 이동", new DialogInterface.OnClickListener() {
                    @Override // 쇼핑몰 이동 버튼 클릭시 실행되는 리스너
                    public void onClick(DialogInterface dialog, int which) {
                        startActivity(new Intent(Intent.ACTION_VIEW, Uri.parse("http://www.tigersshop.kr/")));
                    } // 구단 공식 샵으로 이동
                });
                dlg7.show();
            }
        });
        t8.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                Toast toast1 = Toast.makeText(MainActivity.this, "삼성 라이온즈 홈/원정 유니폼입니다.", Toast.LENGTH_SHORT);
                toast1.setGravity(Gravity.BOTTOM, 0, 300); toast1.show(); // (대화상자 열릴때)구단로고와 텍스트가 겹치지 않도록 위치 조정

                dialogView = (View) View.inflate(MainActivity.this, R.layout.dialog_main, null);
                // dialog_main.xml 파일을 인플레이트 해서 dialogView에 대입
                select = (ImageView) dialogView.findViewById(R.id.select);
                // 대화상자의 이미지 뷰에 접근

                AlertDialog.Builder dlg8 = new AlertDialog.Builder(MainActivity.this);
                select.setImageResource(R.drawable.uniform_lions); // 이미지 뷰에 삼성 유니폼 이미지 접목

                dlg8.setTitle("삼성 라이온즈"); // 제목 설정
                dlg8.setIcon(R.drawable.logo_lions); // 아이콘 설정
                dlg8.setView(dialogView); // 인플레이트 한 뷰를 대화상자로 설정
                dlg8.setPositiveButton("닫기", new DialogInterface.OnClickListener() {
                    @Override // 닫기 버튼 클릭시 실행되는 리스너
                    public void onClick(DialogInterface dialog, int which) {
                        Toast toast2 = Toast.makeText(MainActivity.this, "대화 상자를 종료합니다.", Toast.LENGTH_SHORT);
                        toast2.setGravity(Gravity.BOTTOM, 0, 300); toast2.show(); // (대화상자 닫힐때)구단로고와 텍스트가 겹치지 않도록 위치 조정
                    }
                });
                dlg8.setNegativeButton("쇼핑몰 이동", new DialogInterface.OnClickListener() {
                    @Override // 쇼핑몰 이동 버튼 클릭시 실행되는 리스너
                    public void onClick(DialogInterface dialog, int which) {
                        startActivity(new Intent(Intent.ACTION_VIEW, Uri.parse("http://www.samsunglionsshop.com/")));
                    } // 구단 공식 샵으로 이동
                });
                dlg8.show();
            }
        });
        t9.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                Toast toast1 = Toast.makeText(MainActivity.this, "한화 이글스 홈/원정 유니폼입니다.", Toast.LENGTH_SHORT);
                toast1.setGravity(Gravity.BOTTOM, 0, 300); toast1.show(); // (대화상자 열릴때)구단로고와 텍스트가 겹치지 않도록 위치 조정

                dialogView = (View) View.inflate(MainActivity.this, R.layout.dialog_main, null);
                // dialog_main.xml 파일을 인플레이트 해서 dialogView에 대입
                select = (ImageView) dialogView.findViewById(R.id.select);
                // 대화상자의 이미지 뷰에 접근

                AlertDialog.Builder dlg9 = new AlertDialog.Builder(MainActivity.this);
                select.setImageResource(R.drawable.uniform_eagles); // 이미지 뷰에 한화 유니폼 이미지 접목

                dlg9.setTitle("한화 이글스"); // 제목 설정
                dlg9.setIcon(R.drawable.logo_eagles); // 아이콘 설정
                dlg9.setView(dialogView); // 인플레이트 한 뷰를 대화상자로 설정
                dlg9.setPositiveButton("닫기", new DialogInterface.OnClickListener() {
                    @Override // 닫기 버튼 클릭시 실행되는 리스너
                    public void onClick(DialogInterface dialog, int which) {
                        Toast toast2 = Toast.makeText(MainActivity.this, "대화 상자를 종료합니다.", Toast.LENGTH_SHORT);
                        toast2.setGravity(Gravity.BOTTOM, 0, 300); toast2.show(); // (대화상자 닫힐때)구단로고와 텍스트가 겹치지 않도록 위치 조정
                    }
                });
                dlg9.setNegativeButton("쇼핑몰 이동", new DialogInterface.OnClickListener() {
                    @Override // 쇼핑몰 이동 버튼 클릭시 실행되는 리스너
                    public void onClick(DialogInterface dialog, int which) {
                        startActivity(new Intent(Intent.ACTION_VIEW, Uri.parse("http://eagles.lockerdium.co.kr/")));
                    } // 구단 공식 샵으로 이동
                });
                dlg9.show();
            }
        });
        t10.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                Toast toast1 = Toast.makeText(MainActivity.this, "롯데 자이언츠 홈/원정 유니폼입니다.", Toast.LENGTH_SHORT);
                toast1.setGravity(Gravity.BOTTOM, 0, 300); toast1.show(); // (대화상자 열릴때)구단로고와 텍스트가 겹치지 않도록 위치 조정

                dialogView = (View) View.inflate(MainActivity.this, R.layout.dialog_main, null);
                // dialog_main.xml 파일을 인플레이트 해서 dialogView에 대입
                select = (ImageView) dialogView.findViewById(R.id.select);
                // 대화상자의 이미지 뷰에 접근

                AlertDialog.Builder dlg10 = new AlertDialog.Builder(MainActivity.this);
                select.setImageResource(R.drawable.uniform_giants); // 이미지 뷰에 롯데 유니폼 이미지 접목

                dlg10.setTitle("롯데 자이언츠"); // 제목 설정
                dlg10.setIcon(R.drawable.logo_giants); // 아이콘 설정
                dlg10.setView(dialogView); // 인플레이트 한 뷰를 대화상자로 설정
                dlg10.setPositiveButton("닫기", new DialogInterface.OnClickListener() {
                    @Override // 닫기 버튼 클릭시 실행되는 리스너
                    public void onClick(DialogInterface dialog, int which) {
                        Toast toast2 = Toast.makeText(MainActivity.this, "대화 상자를 종료합니다.", Toast.LENGTH_SHORT);
                        toast2.setGravity(Gravity.BOTTOM, 0, 300); toast2.show(); // (대화상자 닫힐때)구단로고와 텍스트가 겹치지 않도록 위치 조정
                    }
                });
                dlg10.setNegativeButton("쇼핑몰 이동", new DialogInterface.OnClickListener() {
                    @Override // 쇼핑몰 이동 버튼 클릭시 실행되는 리스너
                    public void onClick(DialogInterface dialog, int which) {
                        startActivity(new Intent(Intent.ACTION_VIEW, Uri.parse("http://www.lottegiantsshop.com/")));
                    } // 구단 공식 샵으로 이동
                });
                dlg10.show();
            }
        });
    }
}