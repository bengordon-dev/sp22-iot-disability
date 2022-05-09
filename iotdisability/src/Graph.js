import React from "react";
export default function Graph(props) {
  return (
    <svg height={`${props.height}px`}
      style={{strokeLineJoin: "round", stroke: "#000", fill: "none", backgroundColor: "white", marginBottom: 20, marginRight: 20, display: "inline-block"}}
      width={`${props.width}px`}
      id="svg">
        <text x="50%" dominantBaseline="middle" textAnchor="middle" y="25" fill="black" fontSize={24}>{props.title}</text>
        {props.lines && [...Array(props.lines).keys()].map((line, key) => (
          <line 
            x1="50" x2={`${props.width - 25}`} stroke="black" strokeWidth="2"
            y1={`${props.height - 20 - key/(props.lines - 1)*(props.height - 65)}`} 
            y2= {`${props.height - 20 - key/(props.lines - 1)*(props.height - 65)}`}
          />
        ))}
        {props.lines && [...Array(props.lines).keys()].map((line, key) => (
          <text x={`${20 - 4*`${15 - key/(props.lines - 1)*props.maxVal}`.length}`} y={`${5 + props.height - 20 - key/(props.lines - 1)*(props.height - 65)}`} fill="black" fontSize={18}>
            {key/(props.lines - 1)*props.maxVal}
          </text>
        )) }
        {props.readings && props.readings.slice(-32).map((reading, i) => (
          <circle key={i} cx={`${60 + ((props.width - 60)/32)*i}`} cy={`${props.height - 20 - ((props.height - 65)/props.maxVal)*props.func(reading)}`} r="5" stroke="black" strokeWidth="1" fill={props.color} />
        ))}
    </svg>
  )
}