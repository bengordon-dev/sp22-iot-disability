import logo from './logo.svg';
import './App.css';
import React, {useState, useEffect, useRef} from "react";
import Graph from './Graph';

function useInterval(callback, delay) {
  const savedCallback = useRef();

  // Remember the latest callback.
  useEffect(() => {
    savedCallback.current = callback;
  }, [callback]);

  // Set up the interval.
  useEffect(() => {
    function tick() {
      savedCallback.current();
    }
    if (delay !== null) {
      let id = setInterval(tick, delay);
      return () => clearInterval(id);
    }
  }, [delay]);
}

function delayFunc(dist) {
  let freq = 10.0 + 1.40625*Math.pow(dist/30.0, 1.8);
  return 2000.0/freq;
}

const ip = "10.145.22.28:80"

function App() {
  const [readings, setReadings] = useState([]);
  const [getReadings, setGetReadings] = useState(false);

  function fetchReadings(oldReadings) {
    fetch(`http://${ip}/read`).then(response => response.json()).then(response => {setReadings([...oldReadings, response])})  
  }

  function fetchReadingsTimed(oldState) {
    fetch(`http://${ip}/read`).then(response => response.json()).then(response => setReadings([...oldState, response])) 
    .then(response => setTimeout(() => fetchReadingsTimed([...oldState, response]), 1000)) 
  }

  function addFakeData() {
    let newState = readings;
    newState.push({distance: 50, angle: 5.45});
    setReadings(newState);
  }

  useInterval(() => {
    // Your custom logic here
    fetchReadings(readings);
  }, 500);

  return (
    <div className="App">
      <header className="App-header">
        <p>
          Readings
        </p>

        <div className="graphRow">
          <Graph width={700} height={350} lines={11} title="Distance (cm)" maxVal={200} mb={20} color="green" 
          readings={readings} func={(reading) => (Math.max(0, reading.distance))}/>
        
          {<Graph width={700} height={350} lines={10} title="Angle (degrees)" maxVal={180} mb={20} color="red" 
          readings={readings} func={(reading) => (reading.angle)}/>}
        </div>

        <div className="graphRow">
          {<Graph width={700} height={350} lines={10} title="log2(Distance in cm)" maxVal={9} mb={20} color="green" 
          readings={readings} func={(reading) => (Math.log2(Math.max(reading.distance, 1)))}/>}
        
          <Graph width={700} height={350} lines={11} title="Motor Delay (ms)" maxVal={200} mb={20} color="blue" 
          readings={readings} func={(reading) => (delayFunc(Math.max(0, reading.distance)))}/>
        </div>

        
        <button onClick={() => fetchReadings(readings)}>Fetch one reading</button>
        <button onClick={() => setReadings([])}>Clear Readings</button>
      </header>
    </div>
  );
}

export default App;
