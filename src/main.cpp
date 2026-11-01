#include <SDL3/SDL.h>
#include <cstdlib>
#include <ctime>
#include <cmath>
#include <fstream>

const char* HIGHSCORE_FILE = "highscore.txt";

enum GameState {
    STATE_MENU,
    STATE_GAMEPLAY,
    STATE_GAMEOVER
};

int LoadHighScore() {
    std::ifstream file(HIGHSCORE_FILE);
    int savedScore = 0;
    if (file.is_open()) {
        file >> savedScore;
        file.close();
    }
    return savedScore;
}

void SaveHighScore(int score) {
    std::ofstream file(HIGHSCORE_FILE);
    if (file.is_open()) {
        file << score;
        file.close();
    }
}

void PlayHitSound(SDL_AudioStream* stream) {
    if (!stream) return;

    const int freq = 44100;
    const int durationMs = 80; 
    const int sampleCount = (freq * durationMs) / 1000;
    
    float* buffer = (float*)SDL_malloc(sampleCount * sizeof(float));
    if (!buffer) return;
    
    float frequency = 440.0f;
    for (int i = 0; i < sampleCount; i++) {
        float time = (float)i / freq;
        buffer[i] = (sinf(2.0f * M_PI * frequency * time) > 0.0f) ? 0.08f : -0.08f;
        
        if (i > sampleCount * 0.6f) {
            float fade = 1.0f - ((float)(i - sampleCount * 0.6f) / (sampleCount * 0.4f));
            buffer[i] *= fade;
        }
    }

    SDL_PutAudioStreamData(stream, buffer, sampleCount * sizeof(float));
    SDL_free(buffer);
}

void DrawDigit(SDL_Renderer* r, int digit, float x, float y, float size) {
    SDL_SetRenderDrawColor(r, 0, 255, 130, 255);
    float w = size, h = size * 1.6f;
    
    SDL_FPoint p[6];
    p[0].x = x;     p[0].y = y;
    p[1].x = x + w; p[1].y = y;
    p[2].x = x;     p[2].y = y + h / 2.0f;
    p[3].x = x + w; p[3].y = y + h / 2.0f;
    p[4].x = x;     p[4].y = y + h;
    p[5].x = x + w; p[5].y = y + h;

    auto line = [&](int from, int to) { SDL_RenderLine(r, p[from].x, p[from].y, p[to].x, p[to].y); };
    switch (digit) {
        case 0: line(0,1); line(0,2); line(1,3); line(2,4); line(3,5); line(4,5); break;
        case 1: line(1,3); line(3,5); break;
        case 2: line(0,1); line(1,3); line(2,3); line(2,4); line(4,5); break;
        case 3: line(0,1); line(1,3); line(2,3); line(3,5); line(4,5); break;
        case 4: line(0,2); line(1,3); line(2,3); line(3,5); break;
        case 5: line(0,1); line(0,2); line(2,3); line(3,5); line(4,5); break;
        case 6: line(0,1); line(0,2); line(2,3); line(2,4); line(3,5); line(4,5); break;
        case 7: line(0,1); line(1,3); line(3,5); break;
        case 8: line(0,1); line(0,2); line(1,3); line(2,3); line(2,4); line(3,5); line(4,5); break;
        case 9: line(0,1); line(0,2); line(1,3); line(2,3); line(3,5); line(4,5); break;
    }
}

void DrawNewRecordText(SDL_Renderer* r, float x, float y, float size) {
    SDL_SetRenderDrawColor(r, 255, 215, 0, 255); 
    float w = size, h = size * 1.6f;
    float spacing = size * 1.5f;

    auto drawLetter = [&](const int* lines, int count, float offset) {
        SDL_FPoint p[6];
        p[0].x = x + offset;     p[0].y = y;
        p[1].x = x + offset + w; p[1].y = y;
        p[2].x = x + offset;     p[2].y = y + h / 2.0f;
        p[3].x = x + offset + w; p[3].y = y + h / 2.0f;
        p[4].x = x + offset;     p[4].y = y + h;
        p[5].x = x + offset + w; p[5].y = y + h;

        for (int i = 0; i < count; i += 2) {
            SDL_RenderLine(r, p[lines[i]].x, p[lines[i]].y, p[lines[i+1]].x, p[lines[i+1]].y);
        }
    };

    int n[] = {0,4, 0,5, 1,5}; drawLetter(n, 6, 0 * spacing);
    int e[] = {0,1, 0,4, 2,3, 4,5}; drawLetter(e, 8, 1 * spacing);
    
    auto drawW = [&](float offset) {
        SDL_FPoint p[5];
        p[0].x = x + offset;         p[0].y = y;            
        p[1].x = x + offset + w;     p[1].y = y;            
        p[2].x = x + offset;         p[2].y = y + h;        
        p[3].x = x + offset + w;     p[3].y = y + h;        
        p[4].x = x + offset + w/2.f; p[4].y = y + h/2.f;    
        
        SDL_RenderLine(r, p[0].x, p[0].y, p[2].x, p[2].y); 
        SDL_RenderLine(r, p[1].x, p[1].y, p[3].x, p[3].y); 
        SDL_RenderLine(r, p[2].x, p[2].y, p[4].x, p[4].y); 
        SDL_RenderLine(r, p[3].x, p[3].y, p[4].x, p[4].y); 
    };
    drawW(2 * spacing);
    
    int r_l[] = {0,4, 0,1, 1,3, 2,3, 2,5}; drawLetter(r_l, 10, 4 * spacing);
    int e2[] = {0,1, 0,4, 2,3, 4,5}; drawLetter(e2, 8, 5 * spacing);
    int c[] = {0,1, 0,4, 4,5}; drawLetter(c, 6, 6 * spacing);
    int o[] = {0,1, 0,4, 1,5, 4,5}; drawLetter(o, 8, 7 * spacing);
    int r3[] = {0,4, 0,1, 1,3, 2,3, 2,5}; drawLetter(r3, 10, 8 * spacing);
    int d[] = {0,4, 0,1, 1,5, 4,5}; drawLetter(d, 8, 9 * spacing);
}

void DrawScore(SDL_Renderer* r, int score, float startX, float startY, float size) {
    if (score == 0) { DrawDigit(r, 0, startX, startY, size); return; }
    int temp = score;
    int digits[10]; 
    int count = 0;
    while (temp > 0 && count < 10) { digits[count++] = temp % 10; temp /= 10; }
    for (int i = count - 1; i >= 0; i--) {
        DrawDigit(r, digits[i], startX, startY, size);
        startX += size * 1.4f;
    }
}

void DrawPlayIcon(SDL_Renderer* r, float x, float y, float w, float h) {
    SDL_SetRenderDrawColor(r, 0, 255, 130, 255);
    SDL_RenderLine(r, x + w * 0.35f, y + h * 0.25f, x + w * 0.7f, y + h * 0.5f);
    SDL_RenderLine(r, x + w * 0.7f, y + h * 0.5f, x + w * 0.35f, y + h * 0.75f);
    SDL_RenderLine(r, x + w * 0.35f, y + h * 0.75f, x + w * 0.35f, y + h * 0.25f);
}

void DrawRetryIcon(SDL_Renderer* r, float x, float y, float w, float h) {
    SDL_SetRenderDrawColor(r, 0, 255, 130, 255);
    float cx = x + w / 2.0f;
    float cy = y + h / 2.0f;
    float radius = h * 0.25f;

    SDL_RenderLine(r, cx - radius, cy - radius, cx + radius, cy - radius);
    SDL_RenderLine(r, cx + radius, cy - radius, cx + radius, cy + radius);
    SDL_RenderLine(r, cx + radius, cy + radius, cx - radius, cy + radius);
    SDL_RenderLine(r, cx - radius, cy + radius, cx - radius, cy - radius + 10.0f);

    SDL_RenderLine(r, cx - radius, cy - radius + 10.0f, cx - radius - 15.0f, cy - radius + 25.0f);
    SDL_RenderLine(r, cx - radius, cy - radius + 10.0f, cx - radius + 15.0f, cy - radius + 25.0f);
}

int main(int argc, char* argv[]) {
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO); 
    srand(time(0));

    SDL_Window* window = SDL_CreateWindow("Honor Pad X9 Balanced", 0, 0, SDL_WINDOW_FULLSCREEN);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, NULL);

    SDL_AudioSpec spec;
    spec.format = SDL_AUDIO_F32;
    spec.channels = 1;
    spec.freq = 44100;
    SDL_AudioStream* audioStream = SDL_OpenAudioDeviceStream(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, &spec, NULL, NULL);
    if (audioStream) {
        SDL_ResumeAudioStreamDevice(audioStream);
    }

    GameState state = STATE_MENU;
    int score = 0;          
    int highScore = LoadHighScore(); 

    bool recordBeatenThisRound = false; 
    uint64_t recordTimestamp = 0; 
    int blinkTimer = 0;

    int screenW = 2000, screenH = 1200;
    SDL_GetRenderOutputSize(renderer, &screenW, &screenH);

    // Динамические размеры игровых объектов в зависимости от ширины экрана
    float paddleW = screenW * 0.2f; 
    float paddleH = screenH * 0.035f;
    float ballSize = screenW * 0.035f;
    
    SDL_FRect paddle = { (float)screenW / 2.0f - paddleW / 2.0f, (float)screenH - paddleH - (screenH * 0.08f), paddleW, paddleH };
    SDL_FRect ball = { (float)screenW / 2.0f - ballSize / 2.0f, screenH * 0.08f, ballSize, ballSize }; 
    
    // Скорость адаптируется под высоту экрана
    float ballSpeedY = screenH * 0.0075f;  
    float ballSpeedX = 0.0f;   
    float initialSpeedY = ballSpeedY;
    
    bool isHitEffect = false; 
    int hitTimer = 0;

    SDL_FRect playButton = { (float)screenW / 2.0f - 150.0f, (float)screenH / 2.0f - 50.0f, 300.0f, 100.0f };
    SDL_FRect retryButton = { (float)screenW / 2.0f - 150.0f, (float)screenH / 2.0f - 50.0f, 300.0f, 100.0f };

    bool running = true;
    SDL_Event event;

    auto ResetBall = [&]() {
        SDL_GetRenderOutputSize(renderer, &screenW, &screenH);
        paddleW = screenW * 0.2f;
        paddleH = screenH * 0.035f;
        ballSize = screenW * 0.035f;

        paddle = { (float)screenW / 2.0f - paddleW / 2.0f, (float)screenH - paddleH - (screenH * 0.08f), paddleW, paddleH };
        ball = { (float)screenW / 2.0f - ballSize / 2.0f, screenH * 0.08f, ballSize, ballSize };

        score = 0;
        recordBeatenThisRound = false; 
        recordTimestamp = 0;
        ballSpeedY = screenH * 0.0075f; 
        initialSpeedY = ballSpeedY;
        ballSpeedX = 0.0f; 
        
        playButton = { (float)screenW / 2.0f - 150.0f, (float)screenH / 2.0f - 50.0f, 300.0f, 100.0f };
        retryButton = { (float)screenW / 2.0f - 150.0f, (float)screenH / 2.0f - 50.0f, 300.0f, 100.0f };
    };

    ResetBall();

    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT) {
                running = false;
            }
            
            // Если игрок перевернул экран в процессе, на лету обновляем размеры
            if (event.type == SDL_EVENT_WINDOW_PIXEL_SIZE_CHANGED) {
                SDL_GetRenderOutputSize(renderer, &screenW, &screenH);
                paddle.y = (float)screenH - paddle.h - (screenH * 0.08f);
                playButton = { (float)screenW / 2.0f - 150.0f, (float)screenH / 2.0f - 50.0f, 300.0f, 100.0f };
                retryButton = { (float)screenW / 2.0f - 150.0f, (float)screenH / 2.0f - 50.0f, 300.0f, 100.0f };
            }
            
            if (event.type == SDL_EVENT_FINGER_DOWN || event.type == SDL_EVENT_FINGER_MOTION) {
                float touchX = event.tfinger.x * (float)screenW;
                float touchY = event.tfinger.y * (float)screenH;

                if (state == STATE_MENU) {
                    if (event.type == SDL_EVENT_FINGER_DOWN) {
                        if (touchX >= playButton.x && touchX <= playButton.x + playButton.w &&
touchY >= playButton.y && touchY <= playButton.y + playButton.h) {
ResetBall();
state = STATE_GAMEPLAY;
}
}
}
else if (state == STATE_GAMEOVER) {
if (event.type == SDL_EVENT_FINGER_DOWN) {
if (touchX >= retryButton.x && touchX <= retryButton.x + retryButton.w &&
touchY >= retryButton.y && touchY <= retryButton.y + retryButton.h) {
ResetBall();
state = STATE_GAMEPLAY;
}
}
}
else if (state == STATE_GAMEPLAY) {
paddle.x = touchX - paddle.w / 2.0f;
if (paddle.x < 0) paddle.x = 0;
if (paddle.x > (float)screenW - paddle.w) paddle.x = (float)screenW - paddle.w;
}
}
}
if (state == STATE_GAMEPLAY) {
ball.y += ballSpeedY;
ball.x += ballSpeedX;
if (ball.x < 0) {
ball.x = 0;
