#include <iostream>
#include <vector>
#include <string>
#include <future>
#include <thread>
#include <chrono>
#include <set>
#include <cstdlib>
#include "complex.hpp"
#include "function.hpp"
#include "bmp.hpp"

constexpr auto MAX_STEPS = 1000;

constexpr auto STEPS_PER_EVAL = 2;

constexpr auto accuracy = 0.001;

constexpr auto progressBarLength = 30;

#define NEGATIVE_ZERO_DOUBLE 0x8000000000000000

#ifdef _debug
constexpr auto MULTITHREADED = false;
#else
constexpr auto MULTITHREADED = true;
#endif

struct renderOptions{
    int imgwidth = -1;
    int imgheight = -1;
    int samples = 0;
    double reOffset = NAN;
    double imOffset = NAN;
    double zoom = 0;

    std::string title = "";
    std::string functionString = "";

    bool openOnFinish = true;
    bool pauseOnFinish = true;
    bool useDefaultValues = false;
    bool displayPercent = true;
    bool showAllRoots = false;
};

//Asks the user for input until a valid response is given
bool getInput(std::string question) {
    std::string choice;
    while (true) {
        std::cout << question;
        std::cin >> choice;
        if (choice[0] == 'y' || choice[0] == 'Y') {
            return true;
        }
        else if (choice[0] == 'n' || choice[0] == 'N') {
            return false;
        }
    }
}

//Asks the user for input until a valid response is given
template<typename T>
T getInput(std::string question) {
    bool gotExpr = false;
    T output;
    while (!gotExpr) {
        std::cout << question;
        std::cin >> output;
        if (!std::cin.fail()) gotExpr = true;
        std::cin.clear(); 
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }
    return output;
}


/// @brief negative zero is hard to remove in Ofast
/// @param v pointer to double to use
void removeNegativeZero(double* v){
    unsigned long long* g = (unsigned long long*)(v);
    if(*g == 0x8000000000000000)
        *g = 0;
    
}

/// @brief weird hash funtion that takes in a complex number
/// @param input complex number
/// @return integer
unsigned long long simpleHash(complex input){
    removeNegativeZero(&input.re);
    removeNegativeZero(&input.im);
    unsigned long long output = 0;
    //make a float pointer with the same adress as output
    float* floatptr = (float*)(&output);
    *floatptr = input.re + 0;
    *(floatptr + 1) = input.im + 0;
    return output;
}

/// @brief Go through one iteration of newtons method
/// @param function referance to function to be evaluated
/// @param input input to iterate
/// @return value after one iteration
complex iterate(func& function, complex input) {
    complex a = function.evaluate_function(input);
    //use nextafter to calculate a suitible value of dx based on precicion of floats
    auto dx = complex(nextafterf(input.re,INFINITY) - input.re);
    dx = dx * 10;
    return input - ((dx * a) / (function.evaluate_function(dx + input) - a));
}

/// @brief Find a root of the function by iterating newtons method. Also adds number of iterations taken 
/// @param function reference to funtion object to be evaluated
/// @param input starting point for newtons method
/// @param shading reference to value where number of iterations taken to find root will be stored
/// @return root that is found
complex newtons_method(func& function, complex input, short& shading) {
    complex value[STEPS_PER_EVAL];

    short steps = 0;
    value[0] = input;
    while (steps < MAX_STEPS) {
        for (int j = 1; j < STEPS_PER_EVAL; j++)
        {
            value[j] = iterate(function, value[j - 1]);
            steps++;
        }
        auto difference = abs(value[0] - value[STEPS_PER_EVAL - 1]);
        if (difference.re < accuracy && difference.im < accuracy) {
            break;
        }
        else {
            value[0] = value[STEPS_PER_EVAL - 1];
        }
    }
    if (steps >= MAX_STEPS - 1) {
        return NAN;
    }
    //std::cout << steps;
    shading = steps;
    return value[STEPS_PER_EVAL - 1];
}

/// @brief Evaluates a section of the image
/// @param offset offset of the 0 value of the whole image
/// @param scale scale of the image
/// @param imgheight pixel height of image
/// @param imgwidth pixel width of image
/// @param start x pixel value to start 
/// @param end x pixel value to end
/// @param function function object to evaluate
/// @param values referance to 2d vector of values to output
/// @param shading referance to 2d vector of shading values
/// @param progressCounter referance to progress counter
void evalSection(complex offset, double scale, int imgheight, int imgwidth, int start, int end, func function, std::vector<std::vector<complex>>& values, std::vector<std::vector<short>>& shading, unsigned int& progressCounter) {
    try {
        for (int i = start; i < end; i++) {
            for (int j = 0; j < imgheight; j++)
            {
                values[i][j] = newtons_method(function, complex(i - imgwidth / 2, j - imgheight / 2) * scale + offset, shading[i][j]);
            }
            progressCounter++;
        }
    }
    catch (int exc) {
        if (exc == 5) {
            std::cout << "Evaluation error" << std::endl;
        }
        else {
            std::cout << "Unknown error: " << exc << std::endl;
        }
    }
}

/// @brief outputs the path taken for a specific starting value of newtons method
/// @param function referance to function to be evaluated
void outputpathTaken(func& function){
    complex value, prev;
    value.re = getInput<double>("real componant:");
    value.im = getInput<double>("imaginary componant:");
    short steps = 0;
    while (steps < MAX_STEPS) {
        prev = value;
        value = iterate(function, value);
        std::cout << string(value) << "\n";
        steps++;
        auto difference = abs(value - prev);
        if (difference.re < accuracy && difference.im < accuracy) {
            break;
        }
    }

}

int main(int argc, char* argv[]) {
    if (argc == 2 && std::string(argv[1]) == "-help") {
        std::cout << "Newtons Fractal:" << std::endl;
        std::cout << "Generates a fractal based on a given function. More information can be found at https://wikipedia.org/wiki/Newton_fractal" << std::endl;
        std::cout << "Command line args" << std::endl;
        std::cout << "-help                     display this list" << std::endl;
        std::cout << "-width or -w              the width of the image in pixels            example: -width 1920" << std::endl;
        std::cout << "-height or -h             the height of the image in pixels           example: -height 1080" << std::endl;
        std::cout << "-reoffset or -re          real number offset of the render            example: -reoffset 0.01" << std::endl;
        std::cout << "-imoffset or -im          imaginary offset of the render              example: -imoffset -0.04" << std::endl;
        std::cout << "-zoom or -z               zoom for the render                         example: -zoom 250" << std::endl;
        std::cout << "-function or -f           function to use(don't use spaces)           example: -function \"sin(x)\"" << std::endl;
        std::cout << "-openonfinish or -o       open the function when redering is done     example: -openonfinish y" << std::endl;
        std::cout << "-pauseonfinish or -p      pause the program when redering is done     example: -pauseonfinish n" << std::endl;
        std::cout << "-default or -d            use default values for                      example: -default" << std::endl;
        std::cout << "-nopercent                don't display percent complete              example: -nopercent" << std::endl;
        std::cout << "-showallroots             show all found roots after render is done   example: -nopercent" << std::endl;
        std::cout << "-samplecout or -s         number of samples per pixel                 example: -samplecout 8" << std::endl;
        std::cout << "-title or -t              change the name of the output bmp file      exampleL -title \"img1.bmp\"" << std::endl;
        return 0;
    }

    //Color table
    //TODO: make the program read this data from a file that can be user-provided
    const pixel color[] = {
        pixel("#800000"),
        pixel("#8B0000"),
        pixel("#A52A2A"),
        pixel("#B22222"),
        pixel("#DC143C"),
        pixel("#FF0000"),
        pixel("#FF6347"),
        pixel("#FF7F50"),
        pixel("#CD5C5C"),
        pixel("#F08080"),
        pixel("#E9967A"),
        pixel("#FA8072"),
        pixel("#FFA07A"),
        pixel("#FF4500"),
        pixel("#FF8C00"),
        pixel("#FFA500"),
        pixel("#FFD700"),
        pixel("#B8860B"),
        pixel("#DAA520"),
        pixel("#EEE8AA"),
        pixel("#BDB76B"),
        pixel("#F0E68C"),
        pixel("#808000"),
        pixel("#FFFF00"),
        pixel("#9ACD32"),
        pixel("#556B2F"),
        pixel("#6B8E23"),
        pixel("#7CFC00"),
        pixel("#ADFF2F"),
        pixel("#006400"),
        pixel("#008000"),
        pixel("#228B22"),
        pixel("#00FF00"),
        pixel("#32CD32"),
        pixel("#90EE90"),
        pixel("#98FB98"),
        pixel("#8FBC8F"),
        pixel("#00FA9A"),
        pixel("#00FF7F"),
        pixel("#2E8B57"),
        pixel("#66CDAA"),
        pixel("#3CB371"),
        pixel("#20B2AA"),
        pixel("#2F4F4F"),
        pixel("#008080"),
        pixel("#008B8B"),
        pixel("#00FFFF"),
        pixel("#00CED1"),
        pixel("#40E0D0"),
        pixel("#48D1CC"),
        pixel("#AFEEEE"),
        pixel("#7FFFD4"),
        pixel("#5F9EA0"),
        pixel("#4682B4"),
        pixel("#6495ED"),
        pixel("#00BFFF"),
        pixel("#1E90FF"),
        pixel("#ADD8E6"),
        pixel("#87CEEB"),
        pixel("#87CEFA"),
        pixel("#191970"),
        pixel("#000080"),
        pixel("#00008B"),
        pixel("#0000CD"),
        pixel("#0000FF"),
        pixel("#4169E1"),
        pixel("#8A2BE2"),
        pixel("#4B0082"),
        pixel("#483D8B"),
        pixel("#6A5ACD"),
        pixel("#7B68EE"),
        pixel("#9370DB"),
        pixel("#8B008B"),
        pixel("#9400D3"),
        pixel("#9932CC"),
        pixel("#BA55D3"),
        pixel("#800080"),
        pixel("#D8BFD8"),
        pixel("#DDA0DD"),
        pixel("#EE82EE"),
        pixel("#FF00FF"),
        pixel("#DA70D6"),
        pixel("#C71585"),
        pixel("#DB7093"),
        pixel("#FF1493"),
        pixel("#FF69B4"),
        pixel("#FFB6C1"),
        pixel("#FFC0CB"),
        pixel("#FFE4C4"),
        pixel("#F5DEB3"),
        pixel("#FFFACD"),
        pixel("#8B4513"),
        pixel("#A0522D"),
        pixel("#D2691E"),
        pixel("#CD853F"),
        pixel("#F4A460"),
        pixel("#DEB887"),
        pixel("#D2B48C"),
        pixel("#BC8F8F"),
        pixel("#FFDAB9"),
        pixel("#FFE4E1"),
        pixel("#FFEFD5"),
        pixel("#708090"),
        pixel("#B0C4DE"),
        pixel("#E6E6FA"),
        pixel("#F0FFF0"),
        pixel("#F0F8FF")
    };

    //Initialize default values
    renderOptions options;

    func func;

    //Argument handling
    if (argc > 1) {
        //Run through each argument and check if it defined, set variables as necessary
        for (int i = 0; i < argc; i++) {
            if (std::string(argv[i]) == "-default" || std::string(argv[i]) == "-d") {
                options.imgwidth = 1920;
                options.imgheight = 1080;
                options.samples = 2;
                options.reOffset = 0.000001;
                options.imOffset = 0.000001;
                options.zoom = 400;
            }else if (std::string(argv[i]) == "-width" || std::string(argv[i]) == "-w") {
                options.imgwidth = std::stoi(argv[i + 1]);
                i++;
            }
            else if (std::string(argv[i]) == "-height" || std::string(argv[i]) == "-h") {
                options.imgheight = std::stoi(argv[i + 1]);
                i++;
            }
            else if (std::string(argv[i]) == "-reoffset" || std::string(argv[i]) == "-re") {
                options.reOffset = std::stod(argv[i + 1]);
                i++;
            }
            else if (std::string(argv[i]) == "-imoffset" || std::string(argv[i]) == "-im") {
                options.imOffset = std::stod(argv[i + 1]);
                i++;
            }
            else if (std::string(argv[i]) == "-zoom" || std::string(argv[i]) == "-z") {
                options.zoom = std::stod(argv[i + 1]);
                i++;
            }
            else if (std::string(argv[i]) == "-function" || std::string(argv[i]) == "-f") {
                options.functionString = argv[i + 1];
                i++;
            }
            else if (std::string(argv[i]) == "-samplecout" || std::string(argv[i]) == "-s") {
                options.samples = std::stoi(argv[i + 1]);
                i++;
            }
            else if (std::string(argv[i]) == "-openonfinish" || std::string(argv[i]) == "-o") {
                if (argv[i+1][0] == 'n' || argv[i + 1][0] == 'N' || std::string(argv[i + 1]) == "false") {
                    options.openOnFinish = false;
                    i++;
                }
            }
            else if (std::string(argv[i]) == "-title" || std::string(argv[i]) == "-t") {
                options.title = argv[i + 1];
                i++;
            }
            else if (std::string(argv[i]) == "-pauseonfinish" || std::string(argv[i]) == "-p") {
                if (argv[i + 1][0] == 'n' || argv[i + 1][0] == 'N' || std::string(argv[i + 1]) == "false") {
                    options.pauseOnFinish = false;
                    i++;
                }
            }
            else if (std::string(argv[i]) == "-nopercent") {
                options.displayPercent = false;
            }
            else if (std::string(argv[i]) == "-showallroots") {
                options.showAllRoots = true;
            }
        }
    }

    //if none of the values have been defined, prompt to use defaults
    if (options.imgwidth == -1 && options.imgheight == -1 && isnanIEEE754(options.reOffset) && isnanIEEE754(options.imOffset) && options.zoom == 0) {
        if(getInput("Use default values?(y/n)")){
            options.imgwidth = 1920;
            options.imgheight = 1080;
            options.samples = 2;
            options.reOffset = 0.000001;
            options.imOffset = 0.000001;
            options.zoom = 400;
        }
    }

    //prompt user for each value if it has not been set
    if (options.imgwidth == -1) {
        options.imgwidth  = getInput<int>("Pixel width of the image: ");
    }
    if (options.imgheight == -1) {
        options.imgheight = getInput<int>("Pixel height of the image: ");
    }
    if (isnanIEEE754(options.reOffset)) {
        options.reOffset = getInput<double>("Offset on real axis: ");
    }
    if (isnanIEEE754(options.imOffset)) {
        options.imOffset = getInput<double>("Offset on imaginary axis: ");
    }
    if (options.zoom == 0) {
        options.zoom = getInput<double>("Zoom: ");
    }
    if (options.samples == 0) {
        options.samples = getInput<int>("Samples: ");
    }
    if (options.functionString != "") {
        func.init(options.functionString);
    }else{
        func.init();
    }
    
    //Start program timer
    clock_t start, end;
    start = clock();

    //Initialize scale, offset, root table, and shading table
    double scale = 1/options.zoom;
    complex offset = complex(options.reOffset, -options.imOffset);
    std::vector<std::vector<std::vector<complex>>> valuesTable(options.samples, std::vector<std::vector<complex>>(options.imgwidth, std::vector<complex>(options.imgheight, complex(NAN))));
    std::vector<std::vector<short>> shading(options.imgwidth, std::vector<short>(options.imgheight, 0));
    unsigned int progressCounter = 0;

    auto processor_count = std::thread::hardware_concurrency();
    std::vector<std::future<void>> thread(processor_count);
    std::vector<std::future_status> status(processor_count);
    for( int sample = 0; sample < options.samples; sample++){
        for (int i = 0; i < processor_count; i++)
        {
            int startcol = round(float(i) * (float(options.imgwidth) / float(processor_count)));
            int endcol = round((float(i) + 1.0) * (float(options.imgwidth) / float(processor_count)));
            auto randOffset = complex((double(rand()) / RAND_MAX) - 0.5, (double(rand()) / RAND_MAX) - 0.5) * scale;
            thread[i] = std::async(std::launch::async, evalSection, offset + randOffset, scale, options.imgheight, options.imgwidth, startcol, endcol, func, std::ref(valuesTable[sample]), std::ref(shading), std::ref(progressCounter));
        }
        while(true){
            int total = 0;
            for(int i = 0; i < processor_count; i++)
                total += int(thread[i].wait_for(std::chrono::milliseconds(100)));
            if(total == 0)
                break;
            // [XXXXXXXXXX----------]
            if (options.displayPercent) {
                std::string progressBar = "\r[" ;
                int i;
                for (i = 0; i < round(((float(progressCounter * progressBarLength) / options.imgwidth) / options.samples)); i++)
                {
                    progressBar += "X";
                }
                for (; i < progressBarLength; i++)
                {
                    progressBar += "-";
                }
                progressBar += "] ";
                std::cout << progressBar << progressCounter << "/" << options.imgwidth * options.samples;
            }
        }
        for (int i = 0; i < processor_count; i++)
            thread[i].get();
        
    }
    
    std::cout << "\nGenerating image..." << std::endl;
    std::vector<imgdata> imgdataTable(options.samples, imgdata(options.imgwidth, options.imgheight));


    //Loop through every pixel
    for (int i = 0; i < options.imgwidth; i++)
    {
        for (int j = 0; j < options.imgheight; j++)
        {
            for(int k = 0; k < options.samples; k++){
                //If NAN is found, set the pixel color to be black
                if (isnanIEEE754(valuesTable[k][i][j])) {
                    imgdataTable[k].data[i][j] = pixel(0);
                    continue;
                }
                
                //Picks a color from the color array based on the roots hash and adds shading value
                valuesTable[k][i][j] = complex(round(valuesTable[k][i][j].re/(accuracy*10))*(accuracy*10),round(valuesTable[k][i][j].im/(accuracy*10))*(accuracy*10));
                auto hash = simpleHash(valuesTable[k][i][j]);
                imgdataTable[k].data[i][j] = color[hash % (sizeof(color) / sizeof(*color))] + pixel(shading[i][j] * 2 / options.samples);
            }
            //average the samples
            int r = 0;
            int g = 0;
            int b = 0;
            for( int sample = 0; sample < options.samples; sample++){
                r += imgdataTable[sample].data[i][j].r;
                g += imgdataTable[sample].data[i][j].g;
                b += imgdataTable[sample].data[i][j].b;
            }
            imgdataTable[0].data[i][j].r = r / options.samples;
            imgdataTable[0].data[i][j].g = g / options.samples;
            imgdataTable[0].data[i][j].b = b / options.samples;
        }
    }
    std::set<complex> roots;
    for(std::vector<complex> rows : valuesTable[0]){
        for(complex value : rows){
            if(!isnanIEEE754(value))
                roots.insert(value);
        }
    }
    //Outputs every root if there are less than 10, otherwise output number of roots
    if (roots.size() > 10 && !options.showAllRoots) std::cout << "found " << roots.size() << " roots" << std::endl;
    else {
        for (complex root : roots)
        {
            std::cout << "fount root :" << string(root) << std::endl;
        }
    }

    
    //End timer and output program time
    end = clock();
    double time_taken = double(end - start) / double(CLOCKS_PER_SEC);
    std::cout << "Time taken by program is : " << time_taken << " sec " << std::endl;
    


    //Replances / and * with _ for filename
    for (int i = 0; i < func.function_string.length();i++) {
        if (func.function_string[i] == '/') func.function_string[i] = '~';
        else if (func.function_string[i] == '*') func.function_string[i] = 'X';
    }

    //Write image data to file
    if(options.title == "")
        options.title = func.function_string;
    bmp bmp(options.title + ".bmp");
    if(bmp.writeFile(imgdataTable[0])) std::cout << "Saved file as: " << func.function_string + ".bmp" << std::endl;
    else std::cout << "Error saving file." << std::endl;

    #ifdef _WIN32
    //"Press any key to continue . . ."
    if(options.pauseOnFinish) system("pause");

    //Open the file
    if (options.openOnFinish) system(("\"\"./images/" + bmp.filename + "\"\"").c_str());
    #endif
    
    return 0;
}


/*
TODO:
add gui
add support for the user to add color pallettes
figure out how to make a png instead of bmp
& '.\Newtons Fractal.exe' -d -re 1 -im 0 -z 800 -f xxx-1 -w 2 -h 2 -s 1 should be 1 color in -Ofast
*/