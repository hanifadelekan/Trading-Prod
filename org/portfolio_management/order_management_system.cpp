struct treasury {
    double USDC = 0.0;
    double SOL = 0.0;
    double wealth = 0.0;
    double exposureUSDC = 0.0;
    double exposureSOL = 0.0;
    double exposurewealth = 0.0;
};

double order_dir(double weighted_midprice, double midprice){
    if (weighted_midprice > midprice){
        return midprice - 0.01;
    };
    if (weighted_midprice < midprice){
        return midprice +0.01;
    };
};


void sendorder(double dir, double size, std::string side){

}
void order_manage(double dir, double size, std::string side){
    if (size > 0){
        sendorder(dir,size*portfolio.USDC,side)
    }
}
bool reader_thread_started = false;
    std::thread reader;
if (!reader_thread_started) {
            reader = std::thread([this]() {
                treasury portfolio;
                while (!glfwWindowShouldClose(window_)) {
                    // Directly access GuiApp members safely here:
                    double mid = atomic_bbo_midprice.load(std::memory_order_relaxed);
                    double imb = atomic_bbo_imbalance.load(std::memory_order_relaxed);
                    double dir_ = order_dir(imb,mid);
                    atomic_dir.store(dir_, std::memory_order_relaxed);


                }
                });
            reader_thread_started = true;


            double GuiApp::fair_price_(double level_1_imbalance,double imbalance, double midprice, double midprice_return) {
    return midprice;}
}

double GuiApp::calc_return(double latest_midprice) {
    return (latest_midprice / current_midprice_) - 1;
}

GuiApp::ImbalanceCode GuiApp::get_imbalance_code(double value) {
    if (value < 0.5) return LOW;
    if (value > 0.5) return HIGH;
    return EQUAL;
}

void GuiApp::sendorders(std::string side, std::string orderid, double price, double size) {
    // Placeholder for sending orders
}

void GuiApp::portfolio_manager(double fair_price, double midprice, double current_position, double phantom_position) {
    // Placeholder for portfolio management
}