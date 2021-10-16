package com.astromedicomp.HelloWorld;

import androidx.appcompat.app.AppCompatActivity;

import android.os.Bundle;

import androidx.appcompat.widget.AppCompatTextView;

import android.graphics.Color;

import android.view.Gravity;

public class MainActivity extends AppCompatActivity 
{

    @Override
    protected void onCreate(Bundle savedInstanceState) 
    {
      super.onCreate(savedInstanceState);

      getWindow().getDecorView().setBackgroundColor(Color.rgb(0, 0, 0));		//Object Chaining

      AppCompatTextView MyTextView = new AppCompatTextView(this);

      MyTextView.setText("Hello World !!!");
		  MyTextView.setTextSize(32);      
 		  MyTextView.setTextColor(Color.rgb(0, 255, 0));
 		  MyTextView.setGravity(Gravity.CENTER);
 		  MyTextView.setBackgroundColor(Color.rgb(0, 0, 0));
 		  setContentView(MyTextView);
    }
}