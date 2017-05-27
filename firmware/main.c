//
// Entry point
//
// Copyright 2016 Blinkinlabs, LLC
//

extern void setup();
extern void loop();

int main(void) {
    setup();
    
    for(;;){
        loop();
    }
    
    return 0;
}
