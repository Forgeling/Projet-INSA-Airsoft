package aeldhran.maletteairsoft;

import android.app.ActionBar;
import android.content.Intent;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.support.v7.widget.Toolbar;
import android.view.View;
import android.widget.Button;

public class quickMatch extends BlunoLibrary {

    public Button attackAT;
    public Button defenseAT;
    public Button capture;
    public Button defuse;

    private String message;
    private String configMessage;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_quickmatch);

        Intent intent = getIntent();
        message = intent.getStringExtra(MatchDuration.EXTRA_MESSAGE);

        Toolbar quickMatch = (Toolbar)  findViewById(R.id.quickMatchToolbar);
        setSupportActionBar(quickMatch);

        android.support.v7.app.ActionBar firstMenu_ac = getSupportActionBar();
        firstMenu_ac.setDisplayHomeAsUpEnabled(true);

        attackAT = (Button) findViewById(R.id.ButtonModeAAT);
        defenseAT = (Button) findViewById(R.id.ButtonModeDAT);
        capture = (Button) findViewById(R.id.ButtonModeC);
        defuse = (Button) findViewById(R.id.ButtonModeDT);

    }

    public void setConfig(View view){
        Intent i = new Intent(this, MainActivity.class);
        int viewId = view.getId();
        if(viewId == attackAT.getId()){
            configMessage = "GM:AAT||" + message + "DM:K||TN:2||DT:0"; // desamorçage par clé --> option a revoir et modifié;
        }else if (viewId == defenseAT.getId()){
            configMessage = "GM:DAT||" + message + "DM:K||TN:2||DT:0";
        }else if (viewId == capture.getId()){
            configMessage = "GM:C||" + message + "DM:K||TN:2||DT:0";
        }else{
            configMessage = "GM:DT||" + message + "DM:K||TN:2||DT:0";
        }
        serialSend(configMessage);

        i.addFlags(Intent.FLAG_ACTIVITY_CLEAR_TOP);
        startActivity(i);
    }

    @Override
    public void onConectionStateChange(connectionStateEnum theconnectionStateEnum) {

    }

    @Override
    public void onSerialReceived(String theString) {

    }
}
