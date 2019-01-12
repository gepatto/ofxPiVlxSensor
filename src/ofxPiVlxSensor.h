#include "VL53L1X.h"
#include <atomic>

class ofxPiVlxSensor : public ofThread {


public:
   
    VL53L1X Distance_Sensor;
    bool sensor_enabled;
    int distance = 0;
	std::condition_variable condition;

    ~ofxPiVlxSensor(){
        stop();
        waitForThread(false);
    }

   void setup(uint8_t distanceMode=1){
        
        sensor_enabled = Distance_Sensor.begin();
        if(!sensor_enabled){
            printf("error starting sensor %s","");
        }else{
            Distance_Sensor.setDistanceMode(distanceMode);
            printf("Distance Mode: %i" , distanceMode);
            start();
        }
    }
	
    void exit(){
		stop();
	}

    int getDistance(){
		return distance;
	}

private:
    /// Start the thread.
    void start(){
        startThread();
    }

    void stop(){
        std::unique_lock<std::mutex> lck(mutex);
        stopThread();
        condition.notify_all();
    }

    // the thread functiona 
    void threadedFunction() {
       
        while(isThreadRunning()) {

            if(sensor_enabled){
                Distance_Sensor.startMeasurement();
                while(Distance_Sensor.newDataReady() == false){
                    usleep(5);
                }   
                lock();
                distance =  Distance_Sensor.getDistance();  
                unlock();
            }
        }

        // done
    }  
};