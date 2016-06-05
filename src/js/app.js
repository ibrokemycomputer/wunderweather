var myAPIKey = null;
var customZip = null;
var url = null;
var initialInstall = null;
var locWarn = null;

Pebble.addEventListener('showConfiguration', function() {
  var url = 'https://ibrokemy.computer/pebble';
  Pebble.openURL(url);
});

Pebble.addEventListener('webviewclosed', function(e) {
  var configData = JSON.parse(decodeURIComponent(e.response));
  
  if (configData.temperatureFormat != "NULL") {
    Pebble.sendAppMessage({
      2: configData.temperatureFormat ? 1 : 0
    }, function() {
    }, function() {
    });
  }
  
  if (configData.twentyFourHourFormat != "NULL") {
    Pebble.sendAppMessage({
      4: configData.twentyFourHourFormat
    }, function() {
    }, function() {
    });
  }
  
  if (configData.colorFormat != "NULL") {
    Pebble.sendAppMessage({
      5: configData.colorFormat ? 1 : 0
    }, function() {
    }, function() {
    });
  }
  
  if (configData.apiKey !== "") {
    myAPIKey = configData.apiKey;
    localStorage.setItem('myAPIKey', myAPIKey);
    localStorage.setItem('initialInstall', "no");
  } else  {
    initial_check();
  }
    
  if (configData.zipEnabled === true) {
    localStorage.setItem('zipEnabled', 1);
    if (configData.customZip !== null && configData.customZip !== "") {
      customZip = configData.customZip;
      localStorage.setItem('customZip', customZip);
      var zipAPI = localStorage.getItem('myAPIKey');
      if (zipAPI !== null) {
        getWeather();
      } else {
        missingAPI();
      }
    } else {
      localStorage.getItem('customZip');
      var noZipAPI = localStorage.getItem('myAPIKey');      
      if (customZip === null) {
        if (noZipAPI !== null) {
          getWeather();
        } else {
          missingAPI();
        }
      }
    }
  } else {
    clearZip();
    var noZipXAPI = localStorage.getItem('myAPIKey');      
    if (noZipXAPI !== null) {
      getWeather();
    } else {
      initial_check();
    }
  }
});

function locationSuccess(pos) {
  url = null;
  var zip = localStorage.getItem('customZip');
  if (zip !== null && zip !== "" && zip != "None" && zip != "null") {
    zipWeather();
  } else {
    var lat = (pos.coords.latitude).toFixed(2);
    var lon = (pos.coords.longitude).toFixed(2);
    var useAPI = localStorage.getItem('myAPIKey');
    url = "http://api.wunderground.com/api/" + useAPI + "/conditions/q/" + lat + "," + lon + ".json";
    weatherResponse();   
  }
}

function zipWeather() {
  localStorage.setItem('locWarn', null);
  locWarn = null;
  var useAPI = localStorage.getItem('myAPIKey');
  var zip = localStorage.getItem('customZip');
  url = "http://api.wunderground.com/api/" + useAPI + "/conditions/q/" + zip + ".json";
  weatherResponse();
}

var xhrRequest = function (url, type, callback) {
  var xhr = new XMLHttpRequest();
  xhr.onload = function () {
    callback(this.responseText);
  };
  xhr.open(type, url);
  initial_check();
  if (xhr.status == 404) {
    Pebble.showSimpleNotificationOnPebble("Error", "Cannot connect to Wunderground.");
  } else {
    xhr.send();
  }
};

function weatherResponse() {
    xhrRequest(url, 'GET',
    function(responseText) {
      var icon = "";
      var json = JSON.parse(responseText);
      if (json.response.hasOwnProperty("error")) {
        icon = 0;
        clearZip();
        var weatherError = json.response.error.type;
        var initialInstall = localStorage.getItem('initialInstall');
        if (initialInstall !== null) {
          if (weatherError == "keynotfound" && myAPIKey !== null) {
            var title = "Invalid API";
            var message = "The API Key you entered is invalid";
            Pebble.showSimpleNotificationOnPebble(title, message);
          } else if (weatherError == "querynotfound") {
            var locWarn = localStorage.getItem('locWarn');
            if (locWarn != "no") {
              var ztitle = "Location Error";
              var zmessage = "Weather is unavailable for your location.";
              Pebble.showSimpleNotificationOnPebble(ztitle, zmessage);
              localStorage.setItem('locWarn', "no");
              locWarn = "no";
            }
          } else if (weatherError != "keynotfound" && weatherError != "querynotfound") {
            var xtitle = "Weather Error";
            var xmessage = "Uknown weather error occured. Please contact the developer about this issue.";
            Pebble.showSimpleNotificationOnPebble(xtitle, xmessage);
          }
          var edictionary = {
            "KEY_ICON": icon
          };
          Pebble.sendAppMessage(edictionary,
            function(e) {
            },
            function(e) {
            }
          );
        } else {
          initial_check();
        }
      } else {
        var temperatureF = json.current_observation.temp_f;
        var temperatureC = json.current_observation.temp_c;
        icon = json.current_observation.icon;
        switch(icon) {
          case "clear":
          case "sunny": {
            icon = 1;
            break;
          }
          case "cloudy": {
            icon = 2;
            break;
          }
          case "fog":
          case "hazy": {
            icon = 3;
            break;
          }
          case "chancerain": {
            icon = 4;
            break;
          }
          case "mostlysunny":
          case "mostlycloudy":
          case "partlycloudy":
          case "partlysunny": {
            icon = 5;
            break;
          }
          case "rain": {
            icon = 6;
            break;
          }
          case "chanceflurries":
          case "chancesleet":
          case "chancesnow":
          case "flurries":
          case "sleet":
          case "snow": {
            icon = 7;
            break;
          }
          case "chancetstorms":
          case "tstorms": {
            icon = 8;
            break;
          }
        }
        var dictionary = {
          "KEY_TEMP_F": temperatureF,
          "KEY_TEMP_C": temperatureC,
          "KEY_ICON": icon
        };
        Pebble.sendAppMessage(dictionary,
          function(e) {
          },
          function(e) {
          }
        );
      }
    }
  );
}

function locationError(err) {
  var locZip = localStorage.getItem('zipEnabled');
  var zip = localStorage.getItem('customZip');
  if (locZip != 1) {
    if (zip === null || zip === "" || zip == "None" || zip == "null") {
      clearZip();
      var locWarn = localStorage.getItem('locWarn');
      if (locWarn != "no") {
        localStorage.setItem('locWarn', "no");
        locWarn = "no";
        var title = "Location Error";
        var message = "This watchface requires location to function. Please enable location or set a custom zip code on the settings page to access weather.";
        Pebble.showSimpleNotificationOnPebble(title, message);
      }
    } else {
      initial_check();
      var initInstall = localStorage.getItem('initialInstall');
      if (initInstall !== null) {
        zipWeather();
      }
    }
  } else if (zip !== null && zip !== "" && zip != "None" && zip != "null") {
    initial_check();
    zipWeather();
  }
}

function missingAPI() {
  var title = "API Key Needed";
  var message = "This watchface requires a free API key from Weather Underground. Please visit Settings in the Pebble App to find out more!";
  Pebble.showSimpleNotificationOnPebble(title, message);
}

function clearZip() {
  localStorage.setItem('zipEnabled', null);
  localStorage.setItem('customZip', null);
}

function initial_check() {
  initialInstall = localStorage.getItem('initialInstall');
  if (initialInstall === null && myAPIKey === null) {
    localStorage.setItem('initialInstall', "no");
    initialInstall = "no";
    missingAPI();
  }
}

function getWeather() {
  navigator.geolocation.getCurrentPosition(
    locationSuccess,
    locationError,
    {timeout: 5000, maximumAge: 300000}
  );
}

Pebble.addEventListener('ready',
  function(e) {
    getWeather();
});

Pebble.addEventListener('appmessage',
  function(e) {
    getWeather();
  }                     
);