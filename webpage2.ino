char const *pageString = R"(
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
  <p class='element'></p>
  <div class='volume-buttons'></div>
</body>
  <script>
    // JavaScript code to send GET requests when holding down the buttons
    var intervalId;
    var volUpUrl = '/volup';
    var volDnUrl = '/voldown';
    var volStopUrl = '/stop';

    var eventCounter = 0;

    function sendRequest(url) {
      fetch(`${url}?counter=${eventCounter}`, { method: 'GET',  })
        .then(function(response) {
          // Handle the response if needed
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

    function eventHandler(type, url) {
      var ele = document.querySelector('.element');
      ele.innerHTML = eventCounter;
      sendRequest(url);
      ++eventCounter;
      eventCounter %= 10;
    }
    
    volumeUpButton.addEventListener('touchstart', () => 
      eventHandler('touchstart', volUpUrl));

    volumeUpButton.addEventListener('touchend',() => 
      eventHandler('touchend', volStopUrl));
    
    volumeDownButton.addEventListener('touchstart', () => 
      eventHandler('touchstart', volDnUrl));

    volumeDownButton.addEventListener('touchend',() => 
      eventHandler('touchend', volStopUrl));
    
    // Append the buttons to the volume-buttons container
    var volumeButtonsContainer = document.querySelector('.volume-buttons');
    volumeButtonsContainer.appendChild(volumeUpButton);
    volumeButtonsContainer.appendChild(volumeDownButton);
  </script>
</html>

)";