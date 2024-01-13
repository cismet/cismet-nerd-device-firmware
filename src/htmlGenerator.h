

// HtmlPageGenerator.h
#ifndef HTMLPAGEGENERATOR_H
#define HTMLPAGEGENERATOR_H
#include <Arduino.h>
String generateHtmlPage(const String &currentMode, bool presenceEnabled)
{
  String html = R"(
<!DOCTYPE html>
<html>
  <head>
    <meta charset="UTF-8" />
    <meta name="viewport" content="width=device-width, initial-scale=1.0" />
    <script src="https://cdn.tailwindcss.com"></script>
   
  </head>
  <body onload="updateButtons(')" +
                currentMode + R"('))" + R"(">
    <main class="flex flex-col items-center justify-center h-screen bg-gray-100 p-6">
      <div class="border text-card-foreground w-full max-w-md mx-auto bg-white rounded-xl shadow-md overflow-hidden md:max-w-2xl" data-v0-t="card">
        <div class="p-6">
          <div class="flex flex-col space-y-4">
            <div class="w-32 h-16 md:w-32 md:h-16 mb-4 mx-auto">
              <img src="https://updates.cismet.de//images/osaLogo.png" alt="Logo" />
            </div>
            <center>
              <h1 style="color: #535353" class="text-2xl font-semibold">
                Hack the planet 🤘🏼 
              </h1>
            </center>
)";

  // Modes and their corresponding colors
  const char *modes[] = {"Startup", "Presence", "Tetris", "Breakout"};
  const char *colors[] = {"blue", "green", "red", "yellow"};
  const char *icons[] = {
      R"( <svg
                  xmlns="http://www.w3.org/2000/svg"
                  width="24"
                  height="24"
                  viewBox="0 0 24 24"
                  fill="none"
                  stroke="currentColor"
                  stroke-width="2"
                  stroke-linecap="round"
                  stroke-linejoin="round"
                  class="w-6 h-6 mr-2 text-white"
                >
                  <path
                    d="M4.5 16.5c-1.5 1.26-2 5-2 5s3.74-.5 5-2c.71-.84.7-2.13-.09-2.91a2.18 2.18 0 0 0-2.91-.09z"
                  ></path>
                  <path
                    d="m12 15-3-3a22 22 0 0 1 2-3.95A12.88 12.88 0 0 1 22 2c0 2.72-.78 7.5-6 11a22.35 22.35 0 0 1-4 2z"
                  ></path>
                  <path d="M9 12H4s.55-3.03 2-4c1.62-1.08 5 0 5 0"></path>
                  <path d="M12 15v5s3.03-.55 4-2c1.08-1.62 0-5 0-5"></path>
                </svg> )", // Startup
      R"( <svg
                  xmlns="http://www.w3.org/2000/svg"
                  width="24"
                  height="24"
                  viewBox="0 0 24 24"
                  fill="none"
                  stroke="currentColor"
                  stroke-width="2"
                  stroke-linecap="round"
                  stroke-linejoin="round"
                  class="w-6 h-6 mr-2 text-white"
                >
                  <circle cx="12" cy="5" r="1"></circle>
                  <path d="m9 20 3-6 3 6"></path>
                  <path d="m6 8 6 2 6-2"></path>
                  <path d="M12 10v4"></path>
                </svg>
                     )",   // Presence
      R"( <svg
                  xmlns="http://www.w3.org/2000/svg"
                  width="24"
                  height="24"
                  viewBox="0 0 24 24"
                  fill="none"
                  stroke="currentColor"
                  stroke-width="2"
                  stroke-linecap="round"
                  stroke-linejoin="round"
                  class="w-6 h-6 mr-2 text-white"
                >
                  <rect width="7" height="7" x="14" y="3" rx="1"></rect>
                  <path
                    d="M10 21V8a1 1 0 0 0-1-1H4a1 1 0 0 0-1 1v12a1 1 0 0 0 1 1h12a1 1 0 0 0 1-1v-5a1 1 0 0 0-1-1H3"
                  ></path>
                </svg> )", // Tetris
      R"(  <svg
                  xmlns="http://www.w3.org/2000/svg"
                  width="24"
                  height="24"
                  viewBox="0 0 24 24"
                  fill="none"
                  stroke="currentColor"
                  stroke-width="2"
                  stroke-linecap="round"
                  stroke-linejoin="round"
                  class="w-6 h-6 mr-2 text-white"
                >
                  <path d="M12 13V2l8 4-8 4"></path>
                  <path d="M20.55 10.23A9 9 0 1 1 8 4.94"></path>
                  <path d="M8 10a5 5 0 1 0 8.9 2.02"></path>
                </svg> )", // Breakout

  };
  const int numModes = 4;
  for (int i = 0; i < numModes; i++)
  {
    if (i == 1 && !presenceEnabled)
    {
      continue;
    }
    String mode = modes[i];
    String icon = icons[i];
    String color = colors[i];
    String activeClass = "bg-" + color + "-500";
    String checkmark = (currentMode == mode) ? " ✓" : "";

    html += "<button id=\"button" + mode + "\" onclick=\"setMode('" + mode + "')\" class=\"inline-flex items-center justify-center text-sm font-medium ring-offset-background transition-colors focus-visible:outline-none focus-visible:ring-2 focus-visible:ring-ring focus-visible:ring-offset-2 disabled:pointer-events-none disabled:opacity-50 h-10 w-full py-2 px-4 " + activeClass + " text-white rounded hover:bg-" + color + "-700\">";
    html += icon;
    html += "<span id=\"buttontext" + mode + "\">";
    html += mode;
    html += "</span>";
    html += "</button>";
  }

  html += R"(
            
       <div class="text-right">
        <button onclick="location.href='/config'"  class="inline-flex justify-center py-2 px-4 border border-transparent shadow-sm text-sm font-medium rounded-md text-white bg-blue-500 hover:bg-blue-700 focus:outline-none focus:ring-2 focus:ring-offset-2 focus:ring-blue-500">
          <svg xmlns="http://www.w3.org/2000/svg" width="24" height="24" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round" class="w-6 h-6 text-white" data-id="18"><path d="M12.22 2h-.44a2 2 0 0 0-2 2v.18a2 2 0 0 1-1 1.73l-.43.25a2 2 0 0 1-2 0l-.15-.08a2 2 0 0 0-2.73.73l-.22.38a2 2 0 0 0 .73 2.73l.15.1a2 2 0 0 1 1 1.72v.51a2 2 0 0 1-1 1.74l-.15.09a2 2 0 0 0-.73 2.73l.22.38a2 2 0 0 0 2.73.73l.15-.08a2 2 0 0 1 2 0l.43.25a2 2 0 0 1 1 1.73V20a2 2 0 0 0 2 2h.44a2 2 0 0 0 2-2v-.18a2 2 0 0 1 1-1.73l.43-.25a2 2 0 0 1 2 0l.15.08a2 2 0 0 0 2.73-.73l.22-.39a2 2 0 0 0-.73-2.73l-.15-.08a2 2 0 0 1-1-1.74v-.5a2 2 0 0 1 1-1.74l.15-.09a2 2 0 0 0 .73-2.73l-.22-.38a2 2 0 0 0-2.73-.73l-.15.08a2 2 0 0 1-2 0l-.43-.25a2 2 0 0 1-1-1.73V4a2 2 0 0 0-2-2z"></path><circle cx="12" cy="12" r="3"></circle></svg>
       </button>
        </div>
      </div>
    </main>
  </body>
   <script>
function setMode(mode) {
  updateButtons(mode);

  fetch('/setMode?mode=' + mode)
    .then(response => {
      if (response.ok) {
        console.log('Mode set to ' + mode);
        document.getElementById('currentMode').textContent = 'Current Mode: ' + mode;
        updateButtons(mode);
      }
    })
    .catch(error => console.error('Error setting mode:', error));
}

function updateButtons(activeMode) {
  const modes = ['Startup', 'Presence', 'Tetris', 'Breakout'];
  modes.forEach(mode => {
    const buttontext = document.getElementById('buttontext' + mode);
    if (buttontext) {
      buttontext.innerHTML = mode + (mode === activeMode ? ' ✓' : '');
    }
  });
}
</script>
</html>
)";

  return html;
}
String generateConfigPage(const String &mqtt_server, int mqtt_port, const String &mqtt_user, const String &mqtt_password, const String &mqtt_topic, bool presenceEnabled)
{
  Serial.print("generateConfigPage");
  String checked = presenceEnabled ? "checked" : "";
  String html = R"(
<!DOCTYPE html>
<html>
  <head>
    <meta charset="UTF-8" />
    <meta name="viewport" content="width=device-width, initial-scale=1.0" />
    <script src="https://cdn.tailwindcss.com"></script>
   
  </head>
  <body>
    <main class="flex flex-col items-center justify-center h-screen p-6">
      <div class="border text-card-foreground w-full max-w-md mx-auto rounded-xl shadow-md overflow-hidden md:max-w-2xl  bg-gray-100" data-v0-t="card">
        <div class="p-6">
          <div class="flex flex-col space-y-4">
            <div class="w-32 h-16 md:w-32 md:h-16 mb-4 mx-auto">
              <img src="https://updates.cismet.de//images/osaLogo.png" alt="Logo" />
            </div>
            <center>
              <h1 style="color: #535353" class="text-2xl font-semibold">
                Configuration
              </h1>
            </center>
)";
  html += R"(
      <form  id="myForm" action="/saveConfig" method="post" class="flex flex-col space-y-4">
       <div class="flex items-center">
          <input type="checkbox" id="pres_enab" name="pres_enab" )" +
          checked + R"( class="h-4 w-4 text-indigo-600 focus:ring-indigo-500 border-gray-300 rounded">
          <label for="pres_enab" class="ml-2 block text-sm font-medium text-gray-700">Presence Enabled</label>
        </div>
        <div>
          <label for="mqtt_server" class="block text-sm font-medium text-gray-700">MQTT Server:</label>
          <input type="text" id="mqtt_server" name="mqtt_server" value=")" +
          mqtt_server + R"(" class="mt-1 block w-full rounded-md border-gray-300 shadow-sm focus:border-indigo-300 focus:ring focus:ring-indigo-200 focus:ring-opacity-50">
        </div>
        <div>
          <label for="mqtt_port" class="block text-sm font-medium text-gray-700">MQTT Port:</label>
          <input type="number" id="mqtt_port" name="mqtt_port" value=")" +
          String(mqtt_port) + R"(" class="mt-1 block w-full rounded-md border-gray-300 shadow-sm focus:border-indigo-300 focus:ring focus:ring-indigo-200 focus:ring-opacity-50">
        </div>
        <div>
          <label for="mqtt_user" class="block text-sm font-medium text-gray-700">MQTT User:</label>
          <input type="text" id="mqtt_user" name="mqtt_user" value=")" +
          mqtt_user + R"(" class="mt-1 block w-full rounded-md border-gray-300 shadow-sm focus:border-indigo-300 focus:ring focus:ring-indigo-200 focus:ring-opacity-50">
        </div>
        <div>
          <label for="mqtt_password" class="block text-sm font-medium text-gray-700">MQTT Password:</label>
          <input type="password" id="mqtt_password" name="mqtt_password" value=")" +
          mqtt_password + R"(" class="mt-1 block w-full rounded-md border-gray-300 shadow-sm focus:border-indigo-300 focus:ring focus:ring-indigo-200 focus:ring-opacity-50">
        </div>
        <div>
          <label for="mqtt_topic" class="block text-sm font-medium text-gray-700">MQTT Topic:</label>
          <input type="text" id="mqtt_topic" name="mqtt_topic" value=")" +
          mqtt_topic + R"(" class="mt-1 block w-full rounded-md border-gray-300 shadow-sm focus:border-indigo-300 focus:ring focus:ring-indigo-200 focus:ring-opacity-50">
        </div>
       
       <div class="text-right">
        <button type="button" onclick="location.href='/'" class="inline-flex justify-center py-2 px-4 border border-transparent shadow-sm text-sm font-medium rounded-md text-white bg-blue-200 hover:bg-blue-300 focus:outline-none focus:ring-2 focus:ring-offset-2 focus:ring-blue-500">
           Cancel
       </button>
       <button type="submit" class="inline-flex justify-center py-2 px-4 border border-transparent shadow-sm text-sm font-medium rounded-md text-white bg-blue-500 hover:bg-blue-700 focus:outline-none focus:ring-2 focus:ring-offset-2 focus:ring-blue-500">
           Save
       </button>
      </div>
      </form>
    </div>
    </div>
    </div>
  </main>
</body>

</html>
)";

  return html;
}

#endif // HTMLPAGEGENERATOR_H
