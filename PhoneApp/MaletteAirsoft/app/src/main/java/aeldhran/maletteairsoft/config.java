package aeldhran.maletteairsoft;

import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.support.v7.widget.Toolbar;
import android.view.View;
import android.widget.CheckBox;
import android.widget.RadioButton;

public class config extends BlunoLibrary {

    String msgGM;
    String msgMD;
    String msgDM = "DM:";
    String msgPW;
    String msgDT;
    String msgTN;

    @Override
    protected void onCreate(Bundle savedInstanceState) {

        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_config);

        Toolbar toolbar = (Toolbar) findViewById(R.id.configToolbar);
        setSupportActionBar(toolbar);

        android.support.v7.app.ActionBar firstMenu_ac = getSupportActionBar();
        firstMenu_ac.setDisplayHomeAsUpEnabled(true);

    }

    public void onRadioButtonClicked(View view) {
        boolean checked = ((RadioButton) view).isChecked();
        switch(view.getId()) {
            case R.id.ModeAAT:
                if (checked)
                    msgGM = "GM:AAT";
                    break;
            case R.id.ModeDAT:
                if (checked)
                    msgGM = "GM:DAT";
                    break;
            case R.id.ModeC:
                if (checked)
                    msgGM = "GM:C";
                    break;
            case R.id.ModeDT:
                if (checked)
                    msgGM = "GM:DT";
                    break;
        }
    }

    public void onCheckboxClicked(View view) {
        // Is the view now checked?
        boolean checked = ((CheckBox) view).isChecked();

        // Check which checkbox was clicked
        switch(view.getId()) {
            case R.id.DefusingModeK:
                if (checked){
                    msgDM += "K";
                }
                break;
            case R.id.DefusingModePassword:
                if (checked){
                    msgDM += "PW";
                }
                break;
            // TODO: Veggie sandwich
        }
    }

    public String buildMsg(){
        String msg = "GM:";

        return msg;
    }

    public void send(View view){
        String msg = buildMsg();

        //serialSend();
    }

    @Override
    public void onConectionStateChange(connectionStateEnum theconnectionStateEnum) {

    }

    @Override
    public void onSerialReceived(String theString) {

    }
}
