import logo from './logo.svg';
import './App.css';
import React, {useState, useEffect, useRef} from "react";

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

function App() {
  const [readings, setReadings] = useState([]);
  const [getReadings, setGetReadings] = useState(false);

  function fetchReadings(oldReadings) {
    fetch("http://10.148.110.39:3000/read").then(response => response.json()).then(response => {setReadings([...oldReadings, response])})  
  }

  function fetchReadingsTimed(oldState) {
    fetch("http://10.148.110.39:3000/read").then(response => response.json()).then(response => setReadings([...oldState, response])) 
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
        <svg height={"400px"}
          style={{strokeLineJoin: "round", stroke: "#000", fill: "none", backgroundColor: "white", marginBottom: 50}}
          width={`${875}px`}
          id="svg">
            <line x1="50" y1="200" x2="850" y2="200" stroke="black" strokeWidth="2" />
            <text x="15" y="205" fill="black" fontSize={20}>90</text>
            <line x1="50" y1="20" x2="850" y2="20" stroke="black" strokeWidth="2" />
            <text x="10" y="25" fill="black" fontSize={20}>180</text>
            <line x1="50" y1="380" x2="850" y2="380" stroke="black" strokeWidth="2" />
            <text x="20" y="385" fill="black" fontSize={20}>0</text>
            {readings && readings.slice(-32).map((reading, i) => (
              <circle key={i} cx={`${70 + 25*i}`} cy={`${380 - 2*reading.angle}`} r="5" stroke="black" strokeWidth="1" fill="red" />
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
              <circle key={i} cx={`${70 + 25*i}`} cy={`${380 - 40*Math.log2(reading.distance + 1)}`} r="5" stroke="black" strokeWidth="1" fill="green" />
            ))}
        </svg>
        
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
