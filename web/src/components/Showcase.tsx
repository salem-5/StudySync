"use client";
import { useState, useEffect } from "react";
import ScrollReveal from "./ScrollReveal";

export default function Showcase() {
    const [lightboxImg, setLightboxImg] = useState<string | null>(null);

    useEffect(() => {
        const handleKeyDown = (e: KeyboardEvent) => {
            if (e.key === 'Escape') setLightboxImg(null);
        };

        if (lightboxImg) {
            document.body.style.overflow = 'hidden';
            document.addEventListener('keydown', handleKeyDown);
        } else {
            document.body.style.overflow = '';
        }

        return () => {
            document.body.style.overflow = '';
            document.removeEventListener('keydown', handleKeyDown);
        };
    }, [lightboxImg]);

    const items = [
        { label: "Dashboard", icon: "ph-squares-four", src: "https://raw.githubusercontent.com/salem-5/StudySync/main/client/resources/showcase.png" },
        { label: "AI Tutor", icon: "ph-robot", src: "https://github.com/salem-5/StudySync/raw/main/client/resources/aiTutor.png" }
    ];

    return (
        <>
            <div className={`lightbox-overlay ${lightboxImg ? "active" : ""}`} onClick={() => setLightboxImg(null)}>
                {/* Fix: Only render the image if lightboxImg is not null */}
                {lightboxImg && (
                    /* eslint-disable-next-line @next/next/no-img-element */
                    <img alt="Big View" className="lightbox-img" src={lightboxImg} onClick={(e) => e.stopPropagation()} />
                )}
            </div>

            <section className="showcase-section">
                <div className="showcase-stack">
                    {items.map((item, idx) => (
                        <ScrollReveal key={idx} className="showcase-item">
                            <div className="showcase-banner">
                                <div className="showcase-label"><i className={`ph-fill ${item.icon}`}></i> {item.label}</div>
                                {/* eslint-disable-next-line @next/next/no-img-element */}
                                <img
                                    alt={`${item.label} Interface`}
                                    className="showcase-img"
                                    src={item.src}
                                    onClick={() => setLightboxImg(item.src)}
                                />
                            </div>
                        </ScrollReveal>
                    ))}
                </div>
            </section>
        </>
    );
}