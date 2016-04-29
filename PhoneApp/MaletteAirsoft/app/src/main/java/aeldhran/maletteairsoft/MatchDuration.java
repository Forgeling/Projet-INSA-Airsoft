package aeldhran.maletteairsoft;

import android.content.Intent;
import android.os.Bundle;
import android.support.design.widget.FloatingActionButton;
import android.support.design.widget.Snackbar;
import android.support.v7.app.AppCompatActivity;
import android.support.v7.widget.Toolbar;
import android.view.View;
import android.widget.Button;

public class MatchDuration extends AppCompatActivity {

    public final static int SHORT_DURATION = 5;
    public final static int MEDIUM_DURATION = 7;
    public final static int LONG_DURATION = 10;

    public final static String EXTRA_MESSAGE = "com.aeldhran.maletteairsoft.MESSAGE";

    Button longButton;
    Button mediumButton;
    Button shortButton;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_match_duration);

        Toolbar toolbar = (Toolbar) findViewById(R.id.matchDurationToolbar);
        setSupportActionBar(toolbar);

        android.support.v7.app.ActionBar firstMenu_ac = getSupportActionBar();
        firstMenu_ac.setDisplayHomeAsUpEnabled(true);

        longButton = (Button) findViewById(R.id.longMatch);
        mediumButton = (Button) findViewById(R.id.mediumMatch);
        shortButton = (Button) findViewById(R.id.shortMatch);
    }

    public void setDuration(View view){
        int viewId = view.getId();
        String message;
        Intent intent = new Intent(this, quickMatch.class);
        if(viewId == longButton.getId()){
            message = "MD:" + LONG_DURATION;
        }else if (viewId == shortButton.getId()){
            message = "MD:" + SHORT_DURATION;
        }else{
            message = "MD:" + MEDIUM_DURATION;
        }
        message +=  "||";
        intent.putExtra(EXTRA_MESSAGE, message);
        startActivity(intent);
    }

}
