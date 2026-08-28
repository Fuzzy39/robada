package io.github.fuzzy39.robada.pcApp;



import java.io.IOException;

import javax.swing.Action;

import javafx.animation.KeyFrame;
import javafx.animation.Timeline;
// javafx
import javafx.application.Application;
import javafx.event.ActionEvent;
import javafx.event.EventHandler;
import javafx.geometry.Point2D;
import javafx.scene.Scene;
import javafx.scene.canvas.Canvas;
import javafx.scene.canvas.GraphicsContext;
import javafx.scene.control.Label;
import javafx.scene.layout.StackPane;
import javafx.scene.layout.VBox;
import javafx.scene.paint.Color;
import javafx.stage.Stage;
import javafx.util.Duration;

public class Main extends Application
{

    private InputDevice inputDevice = null;
    Label l;
    
    public static void main(String[] args) 
    {
        launch();
    }


    @Override
    public void start(Stage stage) 
    {

        // default text.
        l = new Label("Controller Not Connected. If you've connected a controller, restart the application.");
        // Canvas canvas = new Canvas(250,250);
        // GraphicsContext gc = canvas.getGraphicsContext2D();

        // gc.setFill(Color.BLUE);
        // gc.fillRect(75,75,100,100);
        
        StickDisplay disp = new StickDisplay(250, ()->update());
         // Try to aquire a controller.
        tryGetController();
       

        Scene scene = new Scene(new VBox(l, disp), 640, 480);
        stage.setScene(scene);
        stage.show();




        // test code
      
    }

    private Point2D update()
    {
        if(inputDevice == null)
        {
            // If we failed to get a controller, JInput will not give us another. Give up.
            return new Point2D(0,0);
        }

        try
        {
            inputDevice.update();
        }
        catch(IOException e)
        {
            // controller was disconnected.
            System.out.println("Disconnect!");
            l.setText("Controller Disconnected. Restart the application with a controller connected.");
            inputDevice = null;
             return new Point2D(0,0);
        }

        System.out.println(inputDevice.getPosition());
        return inputDevice.getPosition(); 
    }

    private void tryGetController()
    {
        if(!InputDevice.getValidControllers().isEmpty())
        {
            inputDevice = new InputDevice(InputDevice.getValidControllers().get(0));
            l.setText("Controller: "+inputDevice.getController().getName());
        }
    }

}