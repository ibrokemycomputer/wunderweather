(function() {
  loadOptions();
  submitHandler();
})();

function submitHandler() {
  var $submitButton = $('#submitButton');

  $submitButton.on('click', function() {
    console.log('Submit');

    var return_to = getQueryParam('return_to', 'pebblejs://close#');
    document.location = return_to + encodeURIComponent(JSON.stringify(getAndStoreConfigData()));
  });
}

function loadOptions() {
  var $colorCheck = $("#colorCheck");
  var $timeFormatCheckbox = $('#timeFormatCheckbox');
  var $temperatureCheck = $("#temperatureCheck");
  var $apiInput = $("#apiInput");
  var $zipEnabled = $("#zipEnabled");
  var $customZip = $("#customZip");

  if (localStorage.twentyFourHourFormat) {
    $timeFormatCheckbox[0].checked = localStorage.twentyFourHourFormat === 'true';
  }
  if (localStorage.temperatureFormat) {
    $temperatureCheck[0].checked = localStorage.temperatureFormat === 'true';
  }
  if (localStorage.colorFormat) {
    $colorCheck[0].checked = localStorage.colorFormat === 'true';
  }
  if (localStorage.zipEnabled) {
    $zipEnabled[0].checked = localStorage.zipEnabled === 'true';
  }
  if (localStorage.apiKey) {
    $("#apiInput").val() == localStorage.apiKey;
  }
  if (localStorage.customZip) {
    $("#customZip").val() == localStorage.customZip;
  }

}

function getAndStoreConfigData() {
  var $timeFormatCheckbox = $('#timeFormatCheckbox');
  var $temperatureCheck = $("#temperatureCheck");
  var $colorCheck = $("#colorCheck");
  var $apiInput = $("#apiInput");
  var $customZip = $("#customZip");
  var $zipEnabled = $("#zipEnabled");

  var options = {
    twentyFourHourFormat: $timeFormatCheckbox[0].checked,
    temperatureFormat: $temperatureCheck[0].checked,
    colorFormat: $colorCheck[0].checked,
    zipEnabled: $zipEnabled[0].checked,
    apiKey: $apiInput.val(),
    customZip: $customZip.val()
  };

  localStorage.twentyFourHourFormat = options.twentyFourHourFormat;
  localStorage.temperatureFormat = options.temperatureFormat;
  localStorage.colorFormat = options.colorFormat;
  localStorage.zipEnabled = options.zipEnabled;
  localStorage.apiKey = options.apiKey;
  localStorage.customZip = options.customZip;

  console.log('Got options: ' + JSON.stringify(options));
  return options;
}

function getQueryParam(variable, defaultValue) {
  var query = location.search.substring(1);
  var vars = query.split('&');
  for (var i = 0; i < vars.length; i++) {
    var pair = vars[i].split('=');
    if (pair[0] === variable) {
      return decodeURIComponent(pair[1]);
    }
  }
  return defaultValue || false;
}

//********Custom Page Interaction**********//
function zipActive()
{
    if($("#zipEnabled").is(":checked"))
        $("#customZipMain").show();
    else
        $("#customZipMain").hide();
}
