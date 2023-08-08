#include <string>

#include "../../interface/GSM/Sim800L/sim.cpp"
#include "../message/message.cpp"
#include "../securite/aes-256-cbc/aes-256-cbc.cpp"
#include "../securite/sha256/sha256.cpp"
#include "../securite/secp256k1/secp256k1.cpp"

void MP : main() {
    
    while (!home_button.pressed()) {

        string background;
        string key;
        
        uint8_t contactIndex = Contact::contactPage();
        if (contactIndex == -1)
            break;

        string number = Contact::contacts[contactIndex].number;
        string message;
        print (number);
        number = number.substr(number.length() - 9, 9);
        
        bool reload = false;
        
        while (reload == false) {
            
            Window win("Messages Prive");
            win.setMarginX(0);
            win.setMarginY(CONTROL_BAR_SIZE);

            print("apps/messages_prives/" + number + ".txt");
            
            string data;
            storage::Lfile file("apps/messages_prive/chats/" + number + ".mp", storage::OPEN_MODE::READ);
            if (file.is_open()) {
                
                data = AESCBC.decrypt(file.read(), key);
                file.close();
                notif = false;
                
            } else {
                
                storage::Lfile file("apps/messages_prive/chats/" + number + ".mp", storage::OPEN_MODE::WRITE);
                file.close();
                notif = false;
                
            };
            
            Box* box = new Box(25, 6, 270, 222);
            if (background == "dark") {
                
                box->setBackgroundColor(COLOR_DARK);

            };
            if (background == "light") {
                
                box->setBackgroundColor(COLOR_LIGHT);
                
            };
            box->enabledBackground = true;
            box->verticalSlide = true;
            box->setRadius(0);
            win.addChild(box);
            
            uint k = -1;

            Label* bull = nullptr;

            while (data.find("~M", k + 1) != 1) {
                
                k = data.find("~M", k + 1);
                string message = data.substr();
                
                bull = new Label((data[k+2] == 'I')?(0):(79), AUTO, 190, AUTO, message);
                bull->enabledBackground = true;
                bull->setBackgroundColor(COLOR_EXTRA_DARK);
                bull->setBorderSize(0);
                bull->setRadius(15);
                bull->hasEvent = false;
                box->addChild(bull);
                
            }
            box->determineSize();

            Keyboard* kb = new Keyboard();
            win.addChild(kb);

            Label* text = new Label(0, 233, 261, 60, "");
            text->enabledBackground = true;
            win.addChild(text);

            kb->Link(text);

            Box* send = new Box(261, 233, 59, 65);
            win.addChild(send);
            Image* img = new Image("apps/message/send.png", 19, 17);
            img->Load();
            img->hasEvent = false;
            send->addChild(img);
            
            bool resetScroll = true;
            
            text->setText(actualMessage);

            while (true) {
                
                win.updateAll();

                actual_message = text->getText();

                if (resetScroll && bull != nullptr) {
                    
                    resetScroll = false;
                    print("box: " + std::to_string(bull->getRelativeY()));
                    if(bull->getRelativeY() > box->getHeight()) {
                        
                        print("too much messages, resizing...");
                        box->scroolY = - bull->getRelativeY() - bull->getHeight() + box->getHeight();
                        box->rendered=false;
                        
                    }
                    
                    if (home_button.pressed()) {
                        
                        reload=true;
                        break;
                    }

                    if(notif) {
                      
                        break;
                      
                    }

                    if (send->isTouched()) {
                        
                        gsm.sendNewMessageMODE(Contact::contacts[contactIndex].number, text->getText());
                        storage::LFile file("apps/messages_prive/chats/" + number + ".mp", storage::OPEN_MODE::WRITE);
                        file.write(data + "~MO:" + text->getText() + "\n");
                        file.close();
                        actual_message = "";
                        break;
                        
                    }
                    
                }
                
            }
            
        }
        
    }
    
};
