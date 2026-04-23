"use client";
import { useState } from "react";

export default function Terminal() {
    const [isClosed, setIsClosed] = useState(false);
    const [isMinimized, setIsMinimized] = useState(false);
    const [isExpanded, setIsExpanded] = useState(false);
    const [isCopied, setIsCopied] = useState(false);

    const handleClose = () => {
        setIsClosed(true);
        setTimeout(() => setIsClosed(false), 2000);
    };

    const handleCopy = async () => {
        const rawCode = `git clone https://github.com/salem-5/StudySync.git\ncd StudySync\ncmake -B build && cmake --build build`;
        try {
            await navigator.clipboard.writeText(rawCode);
            setIsCopied(true);
            setTimeout(() => setIsCopied(false), 2000);
        } catch (err) {
            console.error("Failed to copy!", err);
        }
    };

    return (
        <div className={`build-container ${isClosed ? "closed" : ""} ${isExpanded ? "expanded" : ""}`}>
            <div className="mac-header">
                <div className="mac-dot dot-close" onClick={handleClose} title="Close"></div>
                <div className="mac-dot dot-min" onClick={() => { setIsMinimized(!isMinimized); setIsExpanded(false); }} title="Minimize"></div>
                <div className="mac-dot dot-exp" onClick={() => { setIsExpanded(!isExpanded); setIsMinimized(false); }} title="Expand"></div>
            </div>

            <div className={`code-body ${isMinimized ? "minimized" : ""}`}>
                <button className={`copy-btn ${isCopied ? "copied" : ""}`} onClick={handleCopy} title="Copy to clipboard">
                    {isCopied ? <><i className="ph-bold ph-check"></i> Copied!</> : <><i className="ph-bold ph-copy"></i> Copy</>}
                </button>

                <code>
                    <div className="cmd-line"><span className="cmd">git clone</span> https://github.com/salem-5/StudySync.git</div>
                    <div className="cmd-line"><span className="cmd">cd</span> <span className="dir">StudySync</span></div>
                    <br />
                    <div className="cmd-line"><span className="cmd">cmake</span> -B build &amp;&amp; <span className="cmd">cmake</span> --build build</div>
                </code>
            </div>
        </div>
    );
}