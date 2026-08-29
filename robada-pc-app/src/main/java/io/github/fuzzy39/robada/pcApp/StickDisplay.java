package io.github.fuzzy39.robada.pcApp;

import javafx.animation.KeyFrame;
import javafx.animation.Timeline;
import javafx.event.ActionEvent;
import javafx.geometry.Point2D;
import javafx.scene.canvas.Canvas;
import javafx.scene.canvas.GraphicsContext;
import javafx.scene.paint.Color;
import javafx.util.Duration;

public class StickDisplay extends Canvas
{
    public interface Point2DSupplier
    {
        Point2D getAsPoint2D();
    }

    final double STICK_SIZE_RATIO = .5;
    final double UPDATES_PER_SECOND = 20;

    private Point2DSupplier pointSource;

    public StickDisplay(double size, Point2DSupplier pointSource )
    {
        super(size, size);
        this.pointSource = pointSource;
        draw();

        // set up repeatedly drawing the thing.
        Timeline repeatingTask = new Timeline(
                new KeyFrame(Duration.seconds(1/UPDATES_PER_SECOND),
                (ActionEvent e)->{draw();} 
            ));

        repeatingTask.setCycleCount(Timeline.INDEFINITE);

        
        repeatingTask.play();
        
    } 

    private void draw()
    {
        GraphicsContext gc = this.getGraphicsContext2D();
        double size = this.heightProperty().get();

        // background
        gc.clearRect(0, 0, size, size);

        gc.setFill(Color.color(.2,.25,.3));
        gc.fillOval(0, 0, size, size);

        // the actual thumbstick
        gc.setFill(Color.color(.9,.85,.8));
        double stickSize = STICK_SIZE_RATIO*size;

        // assuming that the position of the thumbstick is anywhere within the unit circle, we want to find the center of a circle
        // of diameter size*STICK_SIZE_RATIO inside a circle of diameter size.
        Point2D center = pointSource.getAsPoint2D().multiply((1-STICK_SIZE_RATIO)*(size/2)).add(new Point2D(size/2, size/2));
        Point2D topLeft = center.subtract(new Point2D(stickSize/2, stickSize/2));   

        gc.fillOval(topLeft.getX(), topLeft.getY(), stickSize, stickSize);     

    }
}
