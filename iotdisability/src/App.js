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

        {/*<svg height={"425px"}
          style={{strokeLineJoin: "round", stroke: "#000", fill: "none", backgroundColor: "white", marginBottom: 50}}
          width={`${875}px`}
          id="svg">
            <text x="400" y="30" fill="black" fontsize={24}>Angle</text>
            <line x1="50" y1="225" x2="850" y2="225" stroke="black" strokeWidth="2" />
            <text x="15" y="230" fill="black" fontSize={20}>90</text>
            <line x1="50" y1="45" x2="850" y2="45" stroke="black" strokeWidth="2" />
            <text x="10" y="50" fill="black" fontSize={20}>180</text>
            <line x1="50" y1="405" x2="850" y2="405" stroke="black" strokeWidth="2" />
            <text x="20" y="410" fill="black" fontSize={20}>0</text>
            {readings && readings.slice(-32).map((reading, i) => (
              <circle key={i} cx={`${70 + 25*i}`} cy={`${405 - 2*reading.angle}`} r="5" stroke="black" strokeWidth="1" fill="red" />
            ))}
        </svg>

        <svg height={"425px"}
          style={{strokeLineJoin: "round", stroke: "#000", fill: "none", backgroundColor: "white", marginBottom: 50}}
          width={`${875}px`}
          id="svg">
            <text x="400" y="30" fill="black" fontsize={24}>Motor delay </text>
            <line x1="50" y1="45" x2="850" y2="45" stroke="black" strokeWidth="2" />
            <text x="10" y="50" fill="black" fontSize={20}>200</text>
            <line x1="50" y1="225" x2="850" y2="225" stroke="black" strokeWidth="2" />
            <text x="15" y="230" fill="black" fontSize={20}>100</text>
            <line x1="50" y1="405" x2="850" y2="405" stroke="black" strokeWidth="2" />
            <text x="20" y="410" fill="black" fontSize={20}>0</text>
            {readings && readings.slice(-32).map((reading, i) => (
              <circle key={i} cx={`${70 + 25*i}`} cy={`${405 - 1.8*delayFunc(Math.max(0, reading.distance))}`} r="5" stroke="black" strokeWidth="1" fill="blue" />
            ))}
        </svg>    

        <svg height={"425px"}
          style={{strokeLineJoin: "round", stroke: "#000", fill: "none", backgroundColor: "white", marginBottom: 50}}
          width={`${875}px`}
          id="svg">
            <text x="400" y="30" fill="black" fontsize={24}>Distance</text>
            <line x1="50" y1="45" x2="850" y2="45" stroke="black" strokeWidth="2" />
            <text x="10" y="50" fill="black" fontSize={20}>450</text>
            <line x1="50" y1="225" x2="850" y2="225" stroke="black" strokeWidth="2" />
            <text x="15" y="230" fill="black" fontSize={20}>225</text>
            <line x1="50" y1="405" x2="850" y2="405" stroke="black" strokeWidth="2" />
            <text x="20" y="410" fill="black" fontSize={20}>0</text>
            {readings && readings.slice(-32).map((reading, i) => (
              <circle key={i} cx={`${70 + 25*i}`} cy={`${405 - 360/450*Math.max(0, reading.distance)}`} r="5" stroke="black" strokeWidth="1" fill="green" />
            ))}
        </svg>     

        <svg height={"400px"}
          style={{strokeLineJoin: "round", stroke: "#000", fill: "none", backgroundColor: "white", marginBottom: 50}}
          width={`${875}px`}
          id="svg">

            <line x1="50" y1="34" x2="850" y2="34" stroke="black" strokeWidth="2" />
            <text x="10" y="38" fill="black" fontSize={16}>400</text>

            <line x1="50" y1="60" x2="850" y2="60" stroke="black" strokeWidth="2" />
            <text x="10" y="64" fill="black" fontSize={16}>256</text>

            <line x1="50" y1="100" x2="850" y2="100" stroke="black" strokeWidth="2" />
            <text x="10" y="104" fill="black" fontSize={16}>128</text>

            <line x1="50" y1="140" x2="850" y2="140" stroke="black" strokeWidth="2" />
            <text x="10" y="144" fill="black" fontSize={16}>64</text>

            <line x1="50" y1="180" x2="850" y2="180" stroke="black" strokeWidth="2" />
            <text x="10" y="184" fill="black" fontSize={16}>32</text>

            <line x1="50" y1="220" x2="850" y2="220" stroke="black" strokeWidth="2" />
            <text x="10" y="224" fill="black" fontSize={16}>16</text>

            <line x1="50" y1="260" x2="850" y2="260" stroke="black" strokeWidth="2" />
            <text x="10" y="264" fill="black" fontSize={16}>8</text>

            <line x1="50" y1="300" x2="850" y2="300" stroke="black" strokeWidth="2" />
            <text x="10" y="304" fill="black" fontSize={16}>4</text>

            <line x1="50" y1="340" x2="850" y2="340" stroke="black" strokeWidth="2" />
            <text x="10" y="344" fill="black" fontSize={16}>2</text>

            <line x1="50" y1="380" x2="850" y2="380" stroke="black" strokeWidth="2" />
            <text x="10" y="384" fill="black" fontSize={16}>1</text>
            {readings && readings.slice(-32).map((reading, i) => (
              <circle key={i} cx={`${70 + 25*i}`} cy={`${380 - 40*Math.log2(Math.max(reading.distance, 1))}`} r="5" stroke="black" strokeWidth="1" fill="green" />
            ))}
            </svg>*/}
        
        <button onClick={() => fetchReadings(readings)}>Fetch one reading</button>
        {/*<button onClick={() => {
            setInterval()
        }}>
          {getReadings ? "stop" : "start"}
      </button>*/}
        <button onClick={() => setReadings([])}>Clear Readings</button>
      </header>
    </div>
  );
}

export default App;
