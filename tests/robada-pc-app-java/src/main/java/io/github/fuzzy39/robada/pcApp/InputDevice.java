package io.github.fuzzy39.robada.pcApp;

import java.io.IOException;
import java.util.ArrayList;
import java.util.List;

import javafx.geometry.Point2D;
import net.java.games.input.Component;
import net.java.games.input.Controller;
import net.java.games.input.ControllerEnvironment;

public class InputDevice 
{
    private Controller controller;
    private Component[] axes;

    public InputDevice(Controller c)
    {
        // verify this controller makes sense to use (we're looking for essentially a joystick)
        if(!(c.getType()==Controller.Type.GAMEPAD || c.getType()==Controller.Type.STICK))
        {
            throw new IllegalArgumentException("The selected controller is not a gamepad or joystick.");
        }

        axes = new Component[2];
        axes[0] = c.getComponent(Component.Identifier.Axis.X);
        axes[1] = c.getComponent(Component.Identifier.Axis.Y);

        if(axes[0]==null || axes[1] == null)
        {
            throw new IllegalArgumentException("The selected controller does not have an X or Y axis");
        }
        // we're happy.
        this.controller = c;

    }      

    public void update() throws IOException
    {
        if(!controller.poll())
        {
            // this should probably be a custom exception type but I'm lazy.
            throw new IOException("Controller Disconnected!");
        }
    }

    public Point2D getPosition()
    {
        return new Point2D(axes[0].getPollData(), axes[1].getPollData());
    }


    public Controller getController()
    {
        return controller;
    }

    public static List<Controller> getValidControllers()
    {
        Controller[] controllers = ControllerEnvironment.getDefaultEnvironment().getControllers();
        ArrayList<Controller> toReturn = new ArrayList<Controller>();
        
        for(Controller c : controllers)
        {
            if(c.getType()==Controller.Type.GAMEPAD || c.getType()==Controller.Type.STICK)
            {
                toReturn.add(c);
            }
        }

        return toReturn;
    }

}
