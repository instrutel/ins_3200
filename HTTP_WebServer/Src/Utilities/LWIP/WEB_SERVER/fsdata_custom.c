#include "lwip/apps/fs.h"

/* THIS FILE IS DEPRECATED AND WILL BE REMOVED IN THE FUTURE */
/* content was moved to fs.h to simplify #include structure */

#define file_NULL (struct fsdata_file *) NULL

#define DATA_HOME_PAGE_HTML_FILENAME_LENGTH	(14U) //"/index.html\0\0\r\n"
static const unsigned char data_Home_Page_Html[] =
"/index.html\0\r\n\
HTTP/1.0 200 OK\r\n\
Server: lwIP/2.0.0 (http://savannah.nongnu.org/projects/lwip)\r\n\
Content-type: text/html\r\n\r\n\
<html lang=\"en\">\n\
<head>\n\
    <meta charset=\"UTF-8\">\n\
    <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n\
    <title>Home Page</title>\n\
    <style>\n\
        body {\n\
            font-family: Arial, sans-serif;\n\
            background-color: #f4f4f9;\n\
            color: #333;\n\
            margin: 0;\n\
            display: flex;\n\
            flex-direction: column;\n\
            height: 100vh;\n\
            justify-content: center; /* Vertically center content */\n\
            align-items: center; /* Horizontally center content */\n\
            text-align: center;\n\
        }\n\
        h1 {\n\
            color: #4CAF50;\n\
        }\n\
        p {\n\
            font-size: 18px;\n\
        }\n\
        .button {\n\
            padding: 10px 20px;\n\
            background-color: #4CAF50;\n\
            color: white;\n\
            border: none;\n\
            border-radius: 5px;\n\
            text-decoration: none;\n\
            font-size: 16px; /* Adjusted font size */\n\
            margin-top: 20px; /* Added some space between text and button */\n\
        }\n\
        .button:hover {\n\
            background-color: #45a049;\n\
        }\n\
        .header, .footer {\n\
            background-color: #333;\n\
            color: white;\n\
            padding: 10px 0;\n\
            font-size: 16px;\n\
            width: 100%; /* Make header/footer full width */\n\
            text-align: center;\n\
        }\n\
        .footer {\n\
            margin-top: auto;\n\
        }\n\
    </style>\n\
</head>\n\
<body>\n\
    <div class=\"header\">\n\
        <h2>Welcome to HTTP Web Server</h2>\n\
    </div>\n\
    <h1>Home Page</h1>\n\
    <p>This is a basic HTTP web server demo page.</p>\n\
    <a href=\"https://instrutel.com/\" class=\"button\" target=\"_blank\">About Us</a>\n\
    <div class=\"footer\">\n\
        <p>&copy; 2025 STM32 Web Server Demo. All rights reserved.</p>\n\
    </div>\n\
</body>\n\
</html>";

struct fsdata_file file_Home_Page[] = { {
file_NULL,
data_Home_Page_Html,
data_Home_Page_Html + DATA_HOME_PAGE_HTML_FILENAME_LENGTH,
sizeof(data_Home_Page_Html) - DATA_HOME_PAGE_HTML_FILENAME_LENGTH,
1,
} };

#define FS_ROOT file_Home_Page

