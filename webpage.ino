#include "webserver.h"

String const WebServer::s_webpage = R"(
<!DOCTYPE html>
<html>
<head>
  <meta name='viewport' content='width=device-width, initial-scale=1'>
  <style>
    body {
      display: flex;
      flex-direction: column;
      justify-content: center;
      align-items: center;
      height: 100vh;
      margin: 0;
      background-color: #f2f2f2;
    }

    .volume-buttons {
      display: flex;
      flex-direction: column;
      align-items: center;
      margin-top: 30px;
    }

    .volume-button {
      width: 0;
      height: 0;
      border-style: solid;
      border-width: 0 50px 86.6px 50px;
      border-color: transparent transparent #000 transparent;
      display: flex;
      justify-content: center;
      align-items: center;
      background-color: #f2f2f2;
      margin-bottom: 20px;
      touch-action: manipulation;
      cursor: pointer;
      user-select: none;
      border-radius: 4px;
    }

    .volume-down {
      transform: rotate(180deg);
    }

    h1 {
      text-align: center;
      font-size: 24px;
      font-family: Arial, sans-serif;
      margin-bottom: 10px;
    }

    @media screen and (min-width: 480px) {
      .volume-button {
        border-width: 0 75px 129.9px 75px;
        margin-bottom: 30px;
      }

      h1 {
        font-size: 32px;
        margin-bottom: 20px;
      }
    }
  </style>

</head>
<body>
  <h1>Marantz Volume</h1>
  <div class='volume-buttons'></div>
</body>
  <script>
    const volUpUrl = '/volup';
    const volDnUrl = '/voldn';
    const stopUrl  = '/stop';

    function sendRequest(url) {
      fetch(url, { method: 'GET',  })
        .then(function(response) {
          // ignore response
        })
        .catch(function(error) {
          console.log('Error:', error);
        });
    }

    // Create buttons
    var volumeUpButton = document.createElement('a');
    var volumeDownButton = document.createElement('a');

    volumeUpButton.classList.add('volume-button');
    volumeDownButton.classList.add('volume-button', 'volume-down');

    const startEvent = 'touchstart';
    const stopEvent = 'touchend';

    volumeUpButton.addEventListener(startEvent, () => 
      sendRequest(volUpUrl));

    volumeUpButton.addEventListener(stopEvent, () => 
      sendRequest(stopUrl));
    
    volumeDownButton.addEventListener(startEvent, () => 
      sendRequest(volDnUrl));

    volumeDownButton.addEventListener(stopEvent, () => 
      sendRequest(stopUrl));
    
    var volumeButtonsContainer = document.querySelector('.volume-buttons');
    volumeButtonsContainer.appendChild(volumeUpButton);
    volumeButtonsContainer.appendChild(volumeDownButton);
  </script>
</html>

)";