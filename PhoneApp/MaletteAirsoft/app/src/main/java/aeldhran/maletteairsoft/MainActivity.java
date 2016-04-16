package aeldhran.maletteairsoft;

import android.content.Intent;
import android.support.design.widget.Snackbar;
import android.os.Bundle;
import android.support.v7.widget.Toolbar;
import android.view.Menu;
import android.view.MenuInflater;
import android.view.MenuItem;
import android.view.View;

public class MainActivity extends BlunoLibrary {

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_first_menue);

        onCreateProcess();
        serialBegin(115200);

        Toolbar firstMenuToolbar = (Toolbar) findViewById(R.id.firstMenuToolbar);
        firstMenuToolbar.setTitle(R.string.app_name);
        setSupportActionBar(firstMenuToolbar);
    }

    protected void onResume(){
        super.onResume();
        System.out.println("BlUNOActivity onResume");
        onResumeProcess();														//onResume Process by BlunoLibrary
    }

    @Override
    protected void onActivityResult(int requestCode, int resultCode, Intent data) {
        onActivityResultProcess(requestCode, resultCode, data);					//onActivityResult Process by BlunoLibrary
        super.onActivityResult(requestCode, resultCode, data);
    }

    @Override
    protected void onPause() {
        super.onPause();
        onPauseProcess();														//onPause Process by BlunoLibrary
    }

    protected void onStop() {
        super.onStop();
        onStopProcess();														//onStop Process by BlunoLibrary
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        onDestroyProcess();														//onDestroy Process by BlunoLibrary
    }

    public boolean onCreateOptionsMenu(Menu menu) {
        MenuInflater inflater = getMenuInflater();
        inflater.inflate(R.menu.items, menu);
        return super.onCreateOptionsMenu(menu);
    }

    public boolean onOptionsItemSelected(MenuItem item) {
        switch(item.getItemId()){
            case R.id.Connect:
                buttonScanOnClickProcess();
                return true;

            default:
                return super.onOptionsItemSelected(item);

        }
    }



    public void quickMatch(View view){
        Intent intent = new Intent(this, MatchDuration.class);
        startActivity(intent);
    }

    public void config(View view){
        Intent intent = new Intent(this, config.class);
        startActivity(intent);
    }

    public void viewMatch(View view){
        Intent intent = new Intent(this, matchView.class);
        startActivity(intent);
    }

    @Override
    public void onConectionStateChange(connectionStateEnum theconnectionStateEnum) {
        switch (theconnectionStateEnum) {											//Four connection state
            case isConnected:
            Snackbar.make(findViewById(R.id.CLFirstMenu), R.string.Connected, Snackbar.LENGTH_SHORT).show();
			break;
		case isConnecting:
            Snackbar.make(findViewById(R.id.CLFirstMenu), R.string.Connecting, Snackbar.LENGTH_SHORT).show();
			break;
		case isToScan:
            Snackbar.make(findViewById(R.id.CLFirstMenu), R.string.notConnected, Snackbar.LENGTH_SHORT).show();
			break;
		case isDisconnecting:
            Snackbar.make(findViewById(R.id.CLFirstMenu), R.string.deconnection, Snackbar.LENGTH_SHORT).show();
			break;
		default:
			break;
		}
    }

    @Override
    public void onSerialReceived(String theString) {
        /**
         * Reception de l'evolution du chrono de la partie et affichage de celui-ci sur l'aperçut partie.
         */
    }
}
