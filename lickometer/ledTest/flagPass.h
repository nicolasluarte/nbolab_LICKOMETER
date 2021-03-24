class flagPass{
  public:
    flagPass(bool plateActive, bool lickometerActive);
    bool plateActive();
    bool lickometerActive();
    void plateOn();
    void plateOff();
    
  private:
    bool _plateActive;
    bool _lickometerActive;
};

flagPass::flagPass(bool plateActive, bool lickometerActive){
    _plateActive = plateActive;
    _lickometerActive = lickometerActive;
}

bool flagPass::plateActive(){
    return _plateActive;
}

bool flagPass::lickometerActive(){
    return _lickometerActive;
}

void flagPass::plateOn(){
    _plateActive = true;
}

void flagPass::plateOff(){
    _plateActive = false;
}
